/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#pragma once
#include "global.h"

#include <iostream>

#include "ostore/ostoreconfig.h"
#include "ostore/ruleobject.h"
#include "tools/csvreader.h"
#include "tools/interrupt.h"
#include "tools/logger.h"


namespace fos {

	class PolicyReader
	{
	public:
		PolicyReader(std::istream& csv_stream, const CsvReaderConfig& reader_config, f_interrupt_cb interrupt_cb);

		bool next_row(RuleObject& rule);

	private:
		// A reference to the csv reader.
		const CsvReaderConfig& _reader_config;

		// The csv reader.
		csv::CsvReader _reader;

		// A reference to the application logger.
		Logger* const _logger;
	};
}
