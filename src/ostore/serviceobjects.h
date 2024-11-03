/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#pragma once
#include "global.h"

#include "ostore/objectdictionary.h"
#include "ostore/ostoreconfig.h"
#include "ostore/serviceobject.h"
#include "tools/csvreader.h"


namespace fos {

	class ServiceObjects : public ObjectDictionary<ServiceObject>
	{
	public:
		ServiceObjects(const CsvReaderConfig& reader_config);

		virtual const ServiceObject* make_object(const csv::CsvValues& values) const override;
		static const ObjectParameters& parameters();
	};
	
}
