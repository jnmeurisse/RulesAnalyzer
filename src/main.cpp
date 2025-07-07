/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#include <cstdio>
#include <cstring>
#include <memory>
#include <stdexcept>

#include "cli/cli.h"
#include "model/domains.h"
#include "model/ipaddress.h"
#include "ostore/ostoreconfig.h"
#include "tools/logger.h"
#include "tools/io.h"


static const char* VERSION = "1.0";
static const char* DEFAULT_CONFIG_FILENAME = "rulan.cfg";


int main(int ac, char** av)
{
	Logger* logger = Logger::get_logger();
	std::unique_ptr<FileLogWriter> file_writer;

	std::unique_ptr<ConsoleLogWriter> console_writer{ std::make_unique<ConsoleLogWriter>() };
	logger->add_writer(console_writer.get());

	try {
		fos::OstoreConfig config;
		const char* config_filename = DEFAULT_CONFIG_FILENAME;

		for (int arg_idx = 1; arg_idx < ac; arg_idx++) {
			if (std::strcmp(av[arg_idx], "-config") == 0) {
				arg_idx++;
				if (arg_idx < ac)
					config_filename = av[arg_idx];
				else
					throw std::runtime_error("config option requires an argument");
			}
			else {
				throw std::runtime_error("unrecognized option");
			}
		}

		logger->info("starting rulan v%s", VERSION);
		if (!rat::file_exists(config_filename)) {
			logger->warning("configuration file '%s' not found", config_filename);
		}
		else {
			// Load program options from a configuration file.
			logger->info("loading configuration file '%s'", config_filename);
			const auto root_table = cpptoml::parse_file(config_filename);
			config.load(*root_table);
		}

		logger->info("* address model %s, strict parser: %s",
			to_string(config.model_config.ip_model).c_str(),
			config.model_config.strict_ip_parser ? "yes" : "no"
		);
		logger->info("* memory nodes=%d cache=%d",
			config.buddy_config.node_size,
			config.buddy_config.cache_size
		);

		if (config.logger_config.enable) {
			file_writer = std::make_unique<FileLogWriter>();
			if (file_writer->open(config.logger_config.filename))
				logger->add_writer(file_writer.get());
			else
				logger->warning("unable to open log file '%s'", config.logger_config.filename.c_str());
		}

		// Initialize the model domains.
		fwm::Domains& domains = fwm::Domains::get();
		logger->info("allocating memory");
		domains.init_bdd(config.buddy_config.node_size, config.buddy_config.cache_size);

		// Run the command line interpreter.
		cli::Cli cli{ config };
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
