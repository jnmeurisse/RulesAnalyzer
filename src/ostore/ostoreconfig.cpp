/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#include "ostore/ostoreconfig.h"

#include <algorithm>
#include <limits>
#include <stdexcept>
#include "model/ipaddress.h"
#include "tools/strutil.h"
#include "fmt/core.h"


namespace fos {

	OstoreConfig::OstoreConfig() :
		logger_config{ false, "" },
		model_config{},
		buddy_config{ 10000, 1000 },
		loader_config{ { ';' }, false },
		writer_config{ ';' },
		fqdn_resolver_config{ true, true, "rulan.fqdn" }
	{
	}


	void OstoreConfig::load(const cpptoml::table& config_table)
	{
		const auto logger_table = config_table.get_table("logger");
		if (logger_table) {
			load_bool(*logger_table, "enable", logger_config.enable);

			if (logger_config.enable) {
				load_string(*logger_table, "filename", logger_config.filename);
				if (rat::trim(logger_config.filename).size() == 0)
					throw_invalid_parameter("filename", "empty filename");
			}
		}

		const auto core_table = config_table.get_table("core");
		if (core_table) {
			std::string ip_model;
			load_string(*core_table, "ip-model", ip_model);

			if (rat::iequal(ip_model, "ipv4") || rat::iequal(ip_model, ""))
				model_config.ip_model = IPAddressModel::IP4Model;
			else if (rat::iequal(ip_model, "ipv6"))
				model_config.ip_model = IPAddressModel::IP6Model;
			else if (rat::iequal(ip_model, "ipv64"))
				model_config.ip_model = IPAddressModel::IP64Model;
			else
				throw_invalid_parameter("ip-model", fmt::format("'{}' is an invalid IP model", ip_model));

			load_bool(*core_table, "strict-ip-parser", model_config.strict_ip_parser);
		}

		const auto buddy_table = config_table.get_table("buddy");
		if (buddy_table) {
			load_int(*buddy_table, "nodes", buddy_config.node_size);
			load_int(*buddy_table, "cache", buddy_config.cache_size);
		}

		const auto loader_table = config_table.get_table("loader");
		if (loader_table) {
			load_bool(*loader_table, "implicit-deny-all", loader_config.add_implicit_deny_all);
			load_char(*loader_table, "list-delimiter", loader_config.reader_config.csv_list_delimiter);
		}

		const auto writer_table = config_table.get_table("writer");
		if (writer_table) {
			load_char(*writer_table, "list-delimiter", writer_config.csv_list_delimiter);
		}

		const auto fqdn_table = config_table.get_table("fqdn");
		if (fqdn_table) {
			load_bool(*fqdn_table, "enable", fqdn_resolver_config.enable);
			load_bool(*fqdn_table, "cache", fqdn_resolver_config.cache);
			load_string(*fqdn_table, "filename", fqdn_resolver_config.filename);
		}

	}


	void OstoreConfig::throw_invalid_parameter(const std::string& key, const std::string& message)
	{
		throw std::runtime_error(fmt::format(
			"invalid parameter '{}' : {}",
			key,
			message)
		);
	}


	bool OstoreConfig::has_key(const cpptoml::table& table, const std::string& key)
	{
		return table.contains(key);
	}


	void OstoreConfig::load_int(const cpptoml::table& table, const std::string& key, int& value)
	{
		if (has_key(table, key)) {
			const auto result = table.get_as<int>(key);
			if (!result)
				throw_invalid_parameter(key, "syntax error");
			if (*result < 0 || *result > std::numeric_limits<int>::max())
				throw_invalid_parameter(key, "value out of range");

			value = *result;
		}
	}


	void OstoreConfig::load_bool(const cpptoml::table& table, const std::string& key, bool& value)
	{
		if (has_key(table, key)) {
			const auto result = table.get_as<bool>(key);
			if (!result)
				throw_invalid_parameter(key, "syntax error");

			value = *result;
		}
	}


	void OstoreConfig::load_string(const cpptoml::table& table, const std::string& key, std::string& value)
	{
		if (has_key(table, key)) {
			const auto result = table.get_as<std::string>(key);
			if (!result)
				throw_invalid_parameter(key, "syntax error");

			value = *result;
		}
	}


	void OstoreConfig::load_char(const cpptoml::table& table, const std::string& key, char& value)
	{
		if (has_key(table, key)) {
			const auto result = table.get_as<std::string>(key);
			if (!result)
				throw_invalid_parameter(key, "syntax error");
			if (result->size() != 1) {
				throw_invalid_parameter(key, "value out of range");

				value = (*result)[0];
			}
		}
	}

}
