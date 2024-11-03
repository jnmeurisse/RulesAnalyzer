/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#include "ostore/addresspools.h"

#include <cassert>
#include <string>


namespace fos {

	AddressPools::AddressPools(const CsvReaderConfig& reader_config) :
		ObjectDictionary<AddressPool>{ std::string{ "address pools" }, reader_config }
	{
	}


	const AddressPool* AddressPools::make_object(const csv::CsvValues& values) const
	{
		const AddressPool* result = nullptr;

		assert(values.size() == parameters().size());

		if (values[0].empty()) {
			_logger->warning(
				"skip address group, name is empty"
			);
		}
		else if (values[1].empty()) {
			_logger->warning(
				"skip address group '%s', address group is empty",
				values[0].c_str()
			);
		}
		else {
			result = new AddressPool(values[0], split(values[1]));
		}

		return result;
	}


	const ObjectParameters& AddressPools::parameters()
	{
		static ObjectParameters parameters{
			{ "name",    "", false },
			{ "members", "", false }
		};

		return parameters;
	}

}
