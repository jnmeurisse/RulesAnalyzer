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
#include "ostore/applicationpool.h"
#include "tools/csvreader.h"


namespace fos {

	class ApplicationPools : public ObjectDictionary<ApplicationPool>
	{
	public:
		ApplicationPools(const CsvReaderConfig& reader_config);

		virtual const ApplicationPool* make_object(const csv::CsvValues& values) const override;
		static const ObjectParameters& parameters();
	};

}
