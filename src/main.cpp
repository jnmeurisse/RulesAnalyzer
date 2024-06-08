/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#include <cstdio>
#include <memory>
#include <stdexcept>

#include "model/domains.h"
#include "ostore/ostoreconfig.h"
#include "tools/logger.h"
#include "tools/tomlcpp.h"

#include "cli/cli.h"

int main(int ac, char** av)
{
	Logger* logger = Logger::get_logger();
	std::unique_ptr<FileLogWriter> file_writer;

	std::unique_ptr<ConsoleLogWriter> console_writer{ std::make_unique<ConsoleLogWriter>() };
	logger->add_writer(console_writer.get());

	try {
		// Load program options from a configuration file.
		fos::OstoreConfig options;
		const std::string config_filename{ "rulesanalyzer.cfg" };

		const auto result = toml::parseFile(config_filename);
		if (result.table == nullptr) {
			logger->warning("configuration file '%s' not loaded : %s",
				config_filename.c_str(),
				result.errmsg.c_str()
			);
		}
		else {
			logger->info("loading configuration file '%s'", config_filename.c_str());
			options.load(*result.table);
		}

		if (options.logger.enable) {
			file_writer = std::make_unique<FileLogWriter>();
			if (file_writer->open(options.logger.filename))
				logger->add_writer(file_writer.get());
			else
				logger->warning("unable to open log file '%s'", options.logger.filename.c_str());
		}

		// Initialize the model domains.
		fwm::Domains domains;
		logger->info("allocating memory");
		domains.init(options.buddy.node_size, options.buddy.cache_size);

		// Run the command line interpreter.
		cli::Cli cli{ options };
		return cli.run();
	}
	catch (const std::exception& e) {
		logger->error(e.what());
	}
	catch (...) {
		logger->error("fatal error");
	}

	// Close all log writers.
	if (file_writer) {
		logger->remove_writer(file_writer.get());
	}

	console_writer->flush();
	logger->remove_writer(console_writer.get());
}
