/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#include "ostore/urlobjects.h"

#include <cassert>


namespace fos {

	UrlObjects::UrlObjects(const CsvReaderConfig& reader_config) :
		ObjectDictionary<UrlObject>{ std::string{ "urls" }, reader_config }
	{
	}


	const UrlObject* UrlObjects::make_object(const csv::CsvValues& values) const
	{
		const UrlObject* result = nullptr;

		// values[0] contains the URL
		assert(values.size() == parameters().size());

		if (values[0].empty()) {
			_logger->warning("url object skipped, url is not defined");
		}
		else if (values[0] == "any") {
			_logger->warning(
				"url object '%s' can't be overwritten",
				values[0].c_str()
			);
		}
		else {
			result = new UrlObject(values[0]);
		}

		return result;
	}


	const ObjectParameters& UrlObjects::parameters()
	{
		static std::vector<csv::CsvColumn> parameters{
			{ "url", "any", false }
		};

		return parameters;
	}

}
