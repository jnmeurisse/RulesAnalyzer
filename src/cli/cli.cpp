/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#include "cli.h"

#include <exception>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <direct.h>

#include "cli/clibdd.h"
#include "cli/clioptioncmd.h"
#include "cli/os/clios.h"
#include "cli/fw/clifw.h"
#include "cli/nw/clinw.h"

#include "tools/csvparser.h"
#include "tools/strutil.h"

namespace cli {

	Cli::Cli(const OstoreConfig& config) :
		_context{ config },
		CliCommandMap(_context)
	{
		add(CommandKeys{ "os", "ostore" },   new CliOsCommand(_context));
		add(CommandKeys{ "fw", "firewall" }, new CliFwCommand(_context));
		add(CommandKeys{ "nw", "network" },  new CliNwCommand(_context));
		add(CommandKeys{ "opt", "option" },  new CliOptionCommand(_context));
		add("bdd", new CliBddCommand(context));
	}


	int Cli::run()
	{
		context.os.initialize();

		std::string line;
		std::vector<std::string> tokens;

		while (true) {
			std::cout << "> ";
			std::cout.tie();

			if (!std::getline(std::cin, line)) {
				if (std::cin.fail()) {
					std::cin.clear();
					continue;
				}
			}

			try {
				tokens.clear();
				if (split_args(line, tokens) > 0) {
					CliArgs args{ tokens };
					const std::string command{ args.front() };

					if (iequal(command, "quit")) {
						std::cout << "bye" << std::endl;
						break;
					}
					else if (command == "?") {
						std::cout << "help not yet available." << std::endl;
					}
					else if ((iequal(command, "ex") || iequal(command, "exec")) && args.size() == 2) {
						args.pop();
						std::ifstream stream{ args.pop(), std::ifstream::in };
						if (!stream.good())
							std::cout << "can't open file." << std::endl;
						else
							run(stream);
					}
					else if (iequal(command, "cd") && args.size() == 2) {
						args.pop();
						if (_chdir(args.pop().c_str()) != 0)
							std::cout << "error in change dir." << std::endl;
					}
					else if (iequal(command, "pwd") && args.size() == 1) {
						char* cwd = _getcwd(0, 0);
						std::cout << cwd << std::endl;
						free(cwd);
					}
					else {
						try {
							const CliCtrlcGuard ctrlc_guard{ context.ctrlc_handler };
							execute(args, ctrlc_guard);
						}
						catch (const std::out_of_range&) {
							std::cout << "syntax error." << std::endl;
						}
					}
				}
			}
			catch (const interrupt_error& e) {
				std::cout << e.what() << std::endl;
			}
			catch (const std::exception& e) {
				std::cout << e.what() << std::endl;
			}
		}

		context.os.terminate();

		return 0;
	}


	void Cli::run(std::istream& in)
	{
		std::string line;
		std::vector<std::string> tokens;

		while (true) {
			if (!std::getline(in, line))
				break;

			// Skip line if it is a comment.
			if (line.size() > 0 && line[0] == '#')
				continue;

			tokens.clear();
			if (split(line, ' ', tokens) > 0) {
				std::cout << "> " << line << std::endl;

				try {
					const CliCtrlcGuard ctrlc_guard{ context.ctrlc_handler };
					execute(CliArgs{ tokens }, ctrlc_guard);
				}
				catch (const std::out_of_range&) {
					std::cout << "syntax error." << std::endl;
					break;
				}
			}
		}
	}


	size_t Cli::split_args(const std::string& line, std::vector<std::string>& tokens)
	{
		static const csv::CsvParserOptions parser_option{ '\n', '"', '\\', ' ', false, true };

		std::istringstream csv_stream(line);
		csv::CsvParser parser(csv_stream, parser_option);

		return parser.next_record(tokens) ? tokens.size() : 0;
	}

}
