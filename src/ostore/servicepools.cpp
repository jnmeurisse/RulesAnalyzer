/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#include "ostore/servicepools.h"

#include <cassert>
#include <string>


namespace fos {
	
	ServicePools::ServicePools(const CsvReaderConfig& reader_config) :
		ObjectDictionary<ServicePool>{ std::string{ "service pools"}, reader_config }
	{
	}


	const ServicePool* ServicePools::make_object(const csv::CsvValues& values) const
	{
		const ServicePool* result = nullptr;

		assert(values.size() == parameters().size());

		if (values[0].empty()) {
			_logger->warning(
				"skip service group, name is empty"
			);
		}
		else if (values[1].empty()) {
			_logger->warning(
				"skip service group '%s', services not defined",
				values[0].c_str()
			);
		}
		else {
			result = new ServicePool(values[0], split(values[1]));
		}

		return result;
	}


	const ObjectParameters& ServicePools::parameters()
	{
		static ObjectParameters parameters{
			{ "name",    "", false },
			{ "members", "", false }
		};

		return parameters;
	}

}
