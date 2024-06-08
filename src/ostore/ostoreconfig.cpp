/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#include "ostoreconfig.h"

#include <algorithm>
#include <limits>
#include <stdexcept>

#include "tools/strutil.h"
#include "tools/tomlcpp.h"

namespace fos {
	OstoreConfig::OstoreConfig() :
		logger{ false, "" },
		buddy{ 10000, 1000 },
		loader{ { ';' }, false },
		writer{ ';' },
		fqdn_resolver{ true, true, "rulesanalyzer.fqdn" }
	{
	}


	void OstoreConfig::load(const toml::Table& config_table)
	{
		const auto logger_table = config_table.getTable("logger");
		if (logger_table) {
			load_bool(*logger_table, "enable", logger.enable);

			if (logger.enable) {
				load_string(*logger_table, "filename", logger.filename);
				if (trim(logger.filename).size() == 0)
					throw_invalid_parameter("filename", "empty filename");
			}
		}

		const auto buddy_table = config_table.getTable("buddy");
		if (buddy_table) {
			load_int(*buddy_table, "nodes", buddy.node_size);
			load_int(*buddy_table, "cache", buddy.cache_size);
		}

		const auto loader_table = config_table.getTable("loader");
		if (loader_table) {
			load_bool(*loader_table, "implicit_deny_all", loader.add_implicit_deny_all);
			load_char(*loader_table, "list_delimiter", loader.csv_config.list_delimiter);
		}

		const auto writer_table = config_table.getTable("writer");
		if (writer_table) {
			load_char(*writer_table, "list_delimiter", writer.csv_list_delimiter);
		}

		const auto fqdn_table = config_table.getTable("fqdn");
		if (fqdn_table) {
			load_bool(*fqdn_table, "enable", fqdn_resolver.enable);
			load_bool(*fqdn_table, "cache", fqdn_resolver.cache);
			load_string(*fqdn_table, "filename", fqdn_resolver.filename);
		}

	}


	void OstoreConfig::throw_invalid_parameter(const std::string& key, const std::string& message)
	{
		throw std::runtime_error(string_format(
			"invalid parameter '%s' : %s",
			key.c_str(),
			message.c_str())
		);
	}


	bool OstoreConfig::has_key(const toml::Table& table, const std::string& key)
	{
		const auto& keys = table.keys();
		return std::find(keys.begin(), keys.end(), key) != std::end(keys);
	}


	void OstoreConfig::load_int(const toml::Table& table, const std::string& key, int& value)
	{
		if (has_key(table, key)) {
			const auto result = table.getInt(key);

			if (!result.first) {
				throw_invalid_parameter(key, "syntax error");
			}
			else {
				if (result.second < 0 || result.second > std::numeric_limits<int>::max()) {
					throw_invalid_parameter(key, "value out of range");
				}

				value = static_cast<int>(result.second);
			}
		}
	}


	void OstoreConfig::load_bool(const toml::Table & table, const std::string & key, bool& value)
	{
		if (has_key(table, key)) {
			const auto result = table.getBool(key);

			if (!result.first) {
				throw_invalid_parameter(key, "syntax error");
			}

			value = result.second;
		}
	}


	void OstoreConfig::load_string(const toml::Table & table, const std::string & key, std::string& value)
	{
		if (has_key(table, key)) {
			const auto result = table.getString(key);

			if (!result.first) {
				throw_invalid_parameter(key, "syntax error");
			}

			value = result.second;
		}
	}


	void OstoreConfig::load_char(const toml::Table & table, const std::string & key, char& value)
	{
		if (has_key(table, key)) {
			const auto result = table.getString(key);

			if (!result.first) {
				throw_invalid_parameter(key, "syntax error");
			}
			else {
				if (result.second.size() != 1) {
					throw_invalid_parameter(key, "value out of range");
				}

				value = result.second[0];
			}
		}
	}

}
