/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#pragma once

#include "config.h"
#include "addresspool.h"
#include "objectdictionary.h"


namespace fos {

	class AddressPools : public ObjectDictionary<AddressPool>
	{
	public:
		AddressPools(const csv_reader_config& reader_config);

		virtual const AddressPool* make_object(const csv::CsvValues& values) const override;
		static const ObjectParameters& parameters();
	};

}
