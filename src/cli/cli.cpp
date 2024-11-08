/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#include "cli.h"

#include <stdexcept>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>

#if defined(RA_OS_WINDOWS)
   #include <direct.h>
   #define chdir _chdir
   #define getcwd _getcwd
#else
   #include <unistd.h>
#endif

#include <linenoise/linenoise.h>


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
		add("bdd",                           new CliBddCommand(context));
	}


	int Cli::run()
	{
		context.ostore.initialize();

#if defined(RA_OS_LINUX)
		linenoiseHistorySetMaxLen(50);
#endif

		std::string line;
		std::vector<std::string> tokens;

		while (true) {
			line.clear();

#if defined(RA_OS_LINUX)
			do {
				const char *p = linenoise("> ");
				if (p) {
					line = p;
					linenoiseFree((void *)p);
				}
			} while (line.size() == 0);
#else
			std::cout << "> ";
			std::cout.tie();

			if (!std::getline(std::cin, line)) {
				if (std::cin.fail()) {
					std::cin.clear();
					continue;
				}
			}
#endif
			try {
				tokens.clear();
				if (split_args(line, tokens) > 0) {

#if defined(RA_OS_LINUX)
					linenoiseHistoryAdd(line.c_str());
#endif

					CliArgs args{ tokens };
					const std::string command{ args.front() };

					if (rat::iequal(command, "quit")) {
						std::cout << "bye" << std::endl;
						break;
					}
					else if (command == "?") {
						std::cout << "help not yet available." << std::endl;
					}
#if defined(RA_OS_LINUX)
					else if (command == "cls") {
						linenoiseClearScreen();
					}
#endif
					else {
						execute(command, args);
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

		context.ostore.terminate();

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

			// remove cr when script file is created on Windows.
			if (line.size() > 0 && line[line.size()-1] == '\r' )
				 line = line.substr( 0, line.size() - 1 );

			tokens.clear();
			if (split_args(line, tokens) > 0) {
				std::cout << "> " << line << std::endl;

				CliArgs args{ tokens };
				const std::string command{ args.front() };
				execute(command, args);
			}
		}
	}


	void Cli::execute(const std::string& command, CliArgs& args)
	{
		if ((rat::iequal(command, "ex") || rat::iequal(command, "exec")) && args.size() == 2) {
			args.pop();
			std::ifstream stream{ args.pop(), std::ifstream::in };
			if (!stream.good())
				std::cout << "can't open file." << std::endl;
			else
				run(stream);
		}
		else if (rat::iequal(command, "cd") && args.size() == 2) {
			args.pop();
			if (::chdir(args.pop().c_str()) != 0)
				std::cout << "error in change dir." << std::endl;
		}
		else if (rat::iequal(command, "pwd") && args.size() == 1) {
			char* cwd = ::getcwd(0, 0);
			std::cout << cwd << std::endl;
			free(cwd);
		}
		else {
			try {
				const CliCtrlcGuard ctrlc_guard{ context.ctrlc_handler };
				CliCommandMap::execute(args, ctrlc_guard);
			}
			catch (const std::out_of_range&) {
				std::cout << "syntax error." << std::endl;
			}
		}
	}



	size_t Cli::split_args(const std::string& line, std::vector<std::string>& args)
	{
		std::istringstream csv_stream(line);
		csv::CsvParser parser(csv_stream);

		csv::CsvValues tokens;
		int argc = 0;
		if (parser.next_record(tokens)) {
			// The CsvParser does not consider multiple spaces as a single separator.
			// We are now removing empty tokens generated by a sequence of multiple spaces.
			for (const std::string& token : tokens) {
				if (token.length() > 0) {
					args.push_back(token);
					argc++;
				}
			}
		}

		return argc;
	}

}