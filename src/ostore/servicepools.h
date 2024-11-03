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
#include "ostore/servicepool.h"
#include "tools/csvreader.h"


namespace fos {

	class ServicePools : public ObjectDictionary<ServicePool>
	{
	public:
		ServicePools(const CsvReaderConfig& reader_config);

		virtual const ServicePool* make_object(const csv::CsvValues& values) const override;
		static const ObjectParameters& parameters();
	};

}

