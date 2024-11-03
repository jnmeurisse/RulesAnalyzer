/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#pragma once
#include "global.h"

#include <string>
#include "model/mconfig.h"

#define CPPTOML_NO_RTTI
#include "tools/cpptoml.h"


namespace fos {
	using namespace fwm;

	class LoggerConfig {
	public:
		// true when logging is enabled.
		bool enable;

		// logger filename.
		std::string filename;
	};

	class BuddyConfig {
	public:
		// initial number of nodes in the BuDDy node table.
		int node_size;

		// number of cache entries.
		int cache_size;
	};

	class CsvReaderConfig {
	public:
		// list delimiter in .csv file
		char csv_list_delimiter;
	};

	class LoaderConfig {
	public:
		// csv config
		CsvReaderConfig reader_config;

		// Add an implicit deny all policy when loading policies.
		bool add_implicit_deny_all;
	};

	class CsvWriterConfig {
	public:
		// list delimiter in .csv file
		char csv_list_delimiter;
	};

	class FqdnResolverConfig {
	public:
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

		void load(const cpptoml::table& config_table);

		// logger configuration
		LoggerConfig logger_config;

		// application core configuration
		ModelConfig model_config;

		// buddy library configuration
		BuddyConfig buddy_config;

		// loader configuration
		LoaderConfig loader_config;

		// csv writer configuration
		CsvWriterConfig writer_config;

		// fqdn resolver configuration
		FqdnResolverConfig fqdn_resolver_config;

	private:
		static void throw_invalid_parameter(const std::string& key, const std::string& message);
		static bool has_key(const cpptoml::table& table, const std::string& key);
		static void load_int(const cpptoml::table& table, const std::string& key, int& value);
		static void load_bool(const cpptoml::table& table, const std::string& key, bool& value);
		static void load_string(const cpptoml::table& table, const std::string& key, std::string& value);
		static void load_char(const cpptoml::table& table, const std::string& key, char& value);
	};

}
