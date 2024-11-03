/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#include "model/tablewriter.h"

#include <algorithm>
#include <iostream>
#include <iomanip>
#include <memory>
#include <sstream>

#include "tools/strutil.h"


namespace fwm {

	TableWriter::TableWriter(const Table& table) :
		_table{ table }
	{
	}


	TableTxtWriter::TableTxtWriter(const Table& table) :
		TableWriter(table),
		_margin{ 0 },
		_header_visible{ true }
	{
	}


	TableTxtWriter& TableTxtWriter::margin(int margin)
	{
		_margin = margin;
		return *this;
	}


	TableTxtWriter& TableTxtWriter::header(bool visible)
	{
		_header_visible = visible;
		return *this;
	}


	void TableTxtWriter::write(std::ostream& os, f_interrupt_cb interrupt_cb) const
	{
		std::string offset_string(_margin, ' ');
		std::vector<size_t> columns_width;
		std::ostringstream row_border;

		// compute and save columns width
		for (size_t col_num = 0; col_num < _table.col_count(); col_num++) {
			size_t wrap_position = _table.wrap_positions()[col_num];

			if (wrap_position == 0) {
				columns_width.push_back(_table.col_width(col_num));
			}
			else {
				for (size_t row_num = 0; row_num < _table.row_count(); row_num++) {
					const Row& row = _table.get_row(row_num);
					for (size_t lidx = 0; lidx < row.cell(col_num).height(); lidx++) {
						word_wrap(row.cell(col_num).line(lidx), wrap_position);
					}
				}

				columns_width.push_back(wrap_position);
			}
		}

		// prepare a row border
		for (size_t col_num = 0; col_num < _table.col_count(); col_num++) {
			row_border << '+' << std::string(columns_width[col_num], '-');
		}
		row_border << '+';

		// output the border
		os << std::left;
		os << offset_string << row_border.str() << std::endl;
		os << offset_string;
		if (_header_visible) {
			// Output the header
			for (size_t col_num = 0; col_num < _table.col_count(); col_num++) {
				os << '|' << std::setw(columns_width[col_num]) << _table.headers()[col_num];
			}
			os << '|' << std::endl;
			os << offset_string << row_border.str() << std::endl;
		}

		// Output all values
		os << std::left;
		for (size_t row_num = 0; row_num < _table.row_count(); row_num++) {
			const Row& row = _table.get_row(row_num);

			size_t row_height = 0;
			std::vector<const Cell*> cells;
			std::list<std::unique_ptr<Cell>> wrapped_cells;
			for (size_t col_num = 0; col_num < _table.col_count(); col_num++)
			{
				if (columns_width[col_num] > 0) {
					wrapped_cells.push_front(std::make_unique<Cell>());
					Cell* wrapped_cell = wrapped_cells.front().get();

					for (size_t lidx = 0; lidx < row.cell(col_num).height(); lidx++) {
						const std::string wrapped_text = word_wrap(
							row.cell(col_num).line(lidx),
							columns_width[col_num]);
						wrapped_cell->append(wrapped_text);
						wrapped_cell->append_nl();
					}

					cells.push_back(wrapped_cell);
				}
				else {
					cells.push_back(&row.cell(col_num));
				}

				row_height = std::max(row_height, cells.back()->height());
			}

			for (int val_idx = 0; val_idx < row_height; val_idx++) {
				os << offset_string;
				for (int col_num = 0; col_num < _table.col_count(); col_num++) {
					const Cell& cell = *cells[col_num];
					os << '|'
						<< std::setw(columns_width[col_num])
						<< cell.line(val_idx);
				}
				os << '|' << std::endl;

				if (interrupt_cb()) {
					throw interrupt_error("** interrupted **");
				}
			}

			os << offset_string << row_border.str() << std::endl;
		}

		os << std::flush;
	}



	std::string TableTxtWriter::word_wrap(const std::string& text, size_t& line_length)
	{
		std::istringstream words(text);
		std::ostringstream wrapped;
		std::string word;

		if (words >> word) {
			wrapped << word;
			int word_len = static_cast<int>(word.length());

			int space_left = static_cast<int>(line_length) - word_len;
			while (words >> word) {
				int word_len = static_cast<int>(word.length());

				if (space_left < word_len + 1) {
					if (word_len + 1 > line_length)
						line_length = word_len;
					wrapped << '\n' << word;
					space_left = static_cast<int>(line_length) - word_len;
				}
				else {
					wrapped << ' ' << word;
					space_left -= word_len + 1;
				}
			}
		}

		return wrapped.str();
	}


	TableCsvWriter::TableCsvWriter(const Table& table) :
		TableWriter(table),
		_separator{","},
		_nl_delimiter{ "\n" }
	{
	}


	TableCsvWriter& TableCsvWriter::separator(char separator)
	{
		_separator = separator;
		return *this;
	}


	TableCsvWriter& TableCsvWriter::newline(char newline)
	{
		_nl_delimiter = newline;
		return *this;
	}


	void TableCsvWriter::write(std::ostream& os, f_interrupt_cb interrupt_cb) const
	{
		os << rat::strings_join(_table.headers(), _separator, true) << std::endl;

		for (int row_num = 0; row_num < _table.row_count(); row_num++) {
			const Row& row = _table.get_row(row_num);
			std::vector<std::string> values;

			for (int col_num = 0; col_num < _table.col_count(); col_num++) {
				values.push_back(row.cell(col_num).to_string(_nl_delimiter));
			}

			os << rat::strings_join(values, _separator, true) << std::endl;
		}
	}

}
