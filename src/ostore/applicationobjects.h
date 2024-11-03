/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#pragma once
#include "global.h"

#include "ostore/applicationobject.h"
#include "ostore/objectdictionary.h"
#include "ostore/serviceobjects.h"
#include "tools/csvreader.h"


namespace fos {

	class ApplicationObjects : public ObjectDictionary<ApplicationObject>
	{
	public:
		ApplicationObjects(const CsvReaderConfig& reader_config, const ServiceObjects& services);

		virtual const ApplicationObject* make_object(const csv::CsvValues& values) const override;
		static const ObjectParameters& parameters();

	private:
		const ServiceObjects& _services;
	};

}
