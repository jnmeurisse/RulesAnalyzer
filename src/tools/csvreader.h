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
#include <memory>
#include <string>
#include <vector>

#include "tools/csvparser.h"
#include "tools/interrupt.h"


namespace csv {

	struct CsvColumn
	{
		std::string name;
		std::string default_value;
		bool optional;
	};


	class CsvReader : CsvParser
	{
	public:
		CsvReader(std::istream& csv_stream, const std::vector<CsvColumn>& columns, f_interrupt_cb interrupt_cb);

		/* Returns the values from the next row.
		*/
		bool next_row(CsvValues& values);

		/* Returns the current row number
		*/
		inline int row_number() const noexcept { return _row_number; }

	private:
		const std::vector<CsvColumn> _columns;

		// position of the columns in the first row
		std::vector<int> column_position;

		// current row number
		int _row_number;

		// true when end of file was reached
		bool _eof;

		// true when the next_row function must be interrupted
		f_interrupt_cb _is_interrupted;
	};
}
