/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#include "ostore/userpools.h"

#include <cassert>
#include <string>


namespace fos {

	UserPools::UserPools(const CsvReaderConfig& reader_config) :
		ObjectDictionary<UserPool>{ std::string{ "user pools" }, reader_config }
	{
	}


	const UserPool* UserPools::make_object(const csv::CsvValues& values) const 
	{
		const UserPool* result = nullptr;

		assert(values.size() == parameters().size());

		if (values[0].empty()) {
			_logger->warning(
				"skip user group, name is empty"
			);
		}
		else if (values[1].empty()) {
			_logger->warning(
				"skip user group '%s', users not defined",
				values[0].c_str()
			);
		}
		else {
			result = new UserPool(values[0], split(values[1]));
		}

		return result;
	}


	const ObjectParameters& UserPools::parameters()
	{
		static ObjectParameters parameters{
			{ "name",    "", false },
			{ "members", "", false }
		};

		return parameters;
	}

}
