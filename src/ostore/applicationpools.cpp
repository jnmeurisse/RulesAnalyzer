/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#include "ostore/applicationpools.h"

#include <cassert>
#include <string>

namespace fos {

	ApplicationPools::ApplicationPools(const CsvReaderConfig& reader_config) :
		ObjectDictionary<ApplicationPool>{ std::string{ "application pools" }, reader_config }
	{
	}


	const ApplicationPool* ApplicationPools::make_object(const csv::CsvValues& values) const
	{
		const ApplicationPool* result = nullptr;

		assert(values.size() == parameters().size());

		if (values[0].empty()) {
			_logger->warning(
				"skip address group, name is empty"
			);
		}
		else if (values[1].empty()) {
			_logger->warning(
				"skip application group '%s', application members are not defined",
				values[0].c_str()
			);
		}
		else {
			result = new ApplicationPool(values[0], split(values[1]));
		}

		return result;
	}

	const ObjectParameters & ApplicationPools::parameters()
	{
		static ObjectParameters parameters{
			{ "name", "", false },
			{ "members", "", false }
		};

		return parameters;
	}

}
