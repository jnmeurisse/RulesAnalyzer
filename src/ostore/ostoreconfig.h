/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#pragma once
#include <string>
#include "tools/tomlcpp.h"

namespace fos {

	struct logger_config {
		// true when logging is enabled.
		bool enable;

		// logger filename.
		std::string filename;
	};

	struct buddy_config {
		// initial number of nodes in the BuDDy node table.
		int node_size;

		// number of cache entries.
		int cache_size;
	};

	struct csv_reader_config {
		// list delimiter in .csv file
		char list_delimiter;
	};

	struct loader_config {
		// csv config
		csv_reader_config csv_config;

		// Add an implicit deny all policy when loading policies.
		bool add_implicit_deny_all;
	};

	struct csv_writer_config {
		// list delimiter in .csv file
		char csv_list_delimiter;
	};

	struct fqdn_resolver_config {
		// When enabled, fqdn are resolved to IP addresses.
		bool enable;

		// When enabled, fqdn are persisted into a file cache.
		bool cache;

		// cache filename
		std::string filename;
	};


	class OstoreConfig {
	public:
		OstoreConfig();

		void load(const toml::Table& config_table);

		// logger configuration
		logger_config logger;

		// buddy library configuration.
		buddy_config buddy;

		// loader configuration
		loader_config loader;

		// csv writer configuration
		csv_writer_config writer;

		// fqdn resolver configuration
		fqdn_resolver_config fqdn_resolver;

	private:
		static void throw_invalid_parameter(const std::string& key, const std::string& message);
		static bool has_key(const toml::Table& table, const std::string& key);
		static void load_int(const toml::Table& table, const std::string& key, int& value);
		static void load_bool(const toml::Table& table, const std::string& key, bool& value);
		static void load_string(const toml::Table& table, const std::string& key, std::string& value);
		static void load_char(const toml::Table& table, const std::string& key, char& value);
	};

}
