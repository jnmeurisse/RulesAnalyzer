/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#pragma once

#include "config.h"
#include "servicepool.h"
#include "objectdictionary.h"


namespace fos {

	class ServicePools : public ObjectDictionary<ServicePool>
	{
	public:
		ServicePools(const csv_reader_config& reader_config);

		virtual const ServicePool* make_object(const csv::CsvValues& values) const override;
		static const ObjectParameters& parameters();
	};

}

