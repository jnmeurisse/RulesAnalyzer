/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#include "tools/csvreader.h"

#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include "tools/strutil.h"
#include "fmt/core.h"


namespace csv {

	CsvReader::CsvReader(std::istream& csv_stream, const std::vector<CsvColumn>& columns, f_interrupt_cb interrupt_cb):
		CsvParser{ csv_stream, CsvParserOptions{ '"', '\\', ',', false, true } },
		_columns{ columns },
		_eof{ false },
		_is_interrupted{ interrupt_cb }
	{
		// prepare a map column name => column index
		std::map<std::string, int, rat::stri_comparator> header_map;
		std::vector<bool> column_configured;

		for (int idx = 0; idx < _columns.size(); idx++) {
			header_map.insert(std::make_pair(_columns[idx].name, idx));
			column_configured.push_back(false);
		}

		size_t col_index = 0;
		CsvValues headers;

		if (!next_record(headers))
			throw std::runtime_error("csv header not found");

		for (const std::string& header : headers) {
			// next column on the header row
			column_position.push_back(-1);

			// search if this column name is selected
			auto it = header_map.find(header);
			if (it != header_map.end()) {
				if (column_configured[it->second])
					throw std::runtime_error(
						fmt::format(
							"column '{}' is defined more than once",
							header.c_str()
						)
					);

				column_position[col_index] = it->second;
			}

			col_index++;
		}

		_row_number = 1;
	}


	bool CsvReader::next_row(CsvValues& values)
	{
		values.clear();

		if (_is_interrupted())
			throw interrupt_error("** interrupted **");

		if (_eof)
			return false;

		// assign default values
		values.resize(_columns.size());
		for (int idx = 0; idx < _columns.size(); idx++) {
			if (_columns[idx].optional)
				values[idx] = _columns[idx].default_value;
		}

		CsvValues record;
		if (next_record(record)) {
			// a new csv record is available
			_row_number += 1;

			// assign the value into the selected column.
			size_t col_idx = 0;
			for (const std::string& value : record) {
				if (col_idx < column_position.size() && column_position[col_idx] >= 0) {
					const size_t col_pos = column_position[col_idx];
					values[col_pos] = value;
					if (!_columns[col_pos].optional && values[col_pos].size() == 0)
						values[col_pos] = _columns[col_pos].default_value;
				}

				col_idx++;
			}
		}
		else {
			_eof = true;
		}

		return !_eof;
	}
}
