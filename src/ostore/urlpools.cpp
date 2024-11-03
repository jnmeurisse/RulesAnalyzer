/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#include "ostore/urlpools.h"

#include <cassert>
#include <string>


namespace fos {

	UrlPools::UrlPools(const CsvReaderConfig& reader_config) :
		ObjectDictionary<UrlPool>{ std::string{ "url pools" }, reader_config }
	{
	}


	const UrlPool* UrlPools::make_object(const csv::CsvValues& values) const
	{
		const UrlPool* result = nullptr;

		assert(values.size() == parameters().size());

		if (values[0].empty()) {
			_logger->warning(
				"skip url group, name is empty"
			);
		}
		else if (values[1].empty()) {
			_logger->warning(
				"skip url group '%s', users not defined",
				values[0].c_str()
			);
		}
		else {
			result = new UrlPool(values[0], split(values[1]));
		}

		return result;
	}


	const ObjectParameters& UrlPools::parameters()
	{
		static ObjectParameters parameters{
			{ "name",    "", false },
			{ "members", "", false }
		};

		return parameters;
	}

}
