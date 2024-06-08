/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#pragma once

#include "config.h"
#include "applicationobject.h"
#include "objectdictionary.h"

#include "tools/csvreader.h"

namespace fos {

	class ApplicationObjects : public ObjectDictionary<ApplicationObject>
	{
	public:
		ApplicationObjects(const csv_reader_config& reader_config);

		virtual const ApplicationObject* make_object(const csv::CsvValues& values) const override;
		static const ObjectParameters& parameters();
	};

}
