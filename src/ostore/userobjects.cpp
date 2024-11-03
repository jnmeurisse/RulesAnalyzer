/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#include "ostore/userobjects.h"

#include <cassert>


namespace fos {

	UserObjects::UserObjects(const CsvReaderConfig& reader_config) :
		ObjectDictionary<UserObject>{ std::string{ "users" }, reader_config }
	{
	}


	const UserObject* UserObjects::make_object(const csv::CsvValues& values) const
	{
		const UserObject* result = nullptr;

		// values[0] contains the user name
		assert(values.size() == parameters().size());

		if (values[0].empty()) {
			_logger->warning("user object skipped, name is not defined");
		}
		else if (values[0] == "any") {
			_logger->warning(
				"user object '%s' can't be overwritten",
				values[0].c_str()
			);
		}
		else {
			result = new UserObject(values[0]);
		}

		return result;
	}


	const ObjectParameters& UserObjects::parameters()
	{
		static std::vector<csv::CsvColumn> parameters{
			{ "name", "", false }
		};

		return parameters;
	}

}
