/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#pragma once

#include "config.h"
#include "addressobject.h"
#include "fqdnresolver.h"
#include "objectdictionary.h"

#include "tools/csvreader.h"


namespace fos {

	class AddressObjects : public ObjectDictionary<AddressObject>
	{
	public:
		AddressObjects(const csv_reader_config& reader_config, FqdnResolver& fqdn_resolver);

		virtual const AddressObject* get(const std::string& name) const noexcept;
		virtual const AddressObject* make_object(const csv::CsvValues& values) const override;
		static const ObjectParameters& parameters();

	private:
		FqdnResolver& _fqdn_resolver;
	};

}
