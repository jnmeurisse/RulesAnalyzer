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
#include <vector>

#include "tools/strutil.h"


namespace fos {

	struct RuleObject {
		using string_vector = std::vector<std::string>;

		int row_number;
		int id;
		std::string name;
		std::string status;
		std::string action;
		string_vector source_zones;
		string_vector destination_zones;
		string_vector source_addresses;
		bool negate_source_addresses;
		string_vector destination_addresses;
		bool negate_destination_addresses;
		string_vector services;
		string_vector applications;
		string_vector users;
		string_vector urls;

		/* Returns true if application-default is configured on this rule.
		*/
		bool default_app_services() const;

		/* Clears all vector and values.
		*/
		void clear();
	};
}
