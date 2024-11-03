/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#include "model/table.h"

#include <algorithm>
#include <iterator>
#include <stdexcept>
#include "tools/strutil.h"


namespace fwm {

	Table::Headers Table::create_headers(const std::string& col0, const std::vector<std::string>& cols)
	{
		Table::Headers headers;
		headers.reserve(1 + cols.size());

		headers.push_back(col0);
		std::copy(cols.begin(), cols.end(), std::back_inserter(headers));

		return headers;
	}



	Table::Table(const Headers& headers, const WrapPositions& wrap_positions)
	{
		init(headers, wrap_positions);
	}


	Table::Table(const Headers& headers) :
		Table(headers, WrapPositions(headers.size(), 0))
	{
	}


	Table& Table::init(const Headers& headers, const WrapPositions& wrap_positions)
	{
		// is it a valid table size ?
		if (headers.size() != wrap_positions.size())
			throw std::runtime_error("internal error : invalid wrap positions size");

		clear();
		_headers = headers;
		_wrap_positions = wrap_positions;

		return *this;
	}


	Table& Table::clear()
	{
		_headers.clear();
		_rows.clear();
		_wrap_positions.clear();

		return *this;
	}


	Row& Table::add_row()
	{
		_rows.push_back(std::make_unique<Row>(_headers.size()));

		return *_rows.back().get();
	}


	Row& Table::get_row(size_t row_idx)
	{
		// is it a valid row index ?
		if (row_idx >= _rows.size())
			throw std::runtime_error("internal error : invalid row index");

		return *_rows[row_idx].get();
	}


	const Row& Table::get_row(size_t row_idx) const
	{
		if (row_idx >= _rows.size())
			throw std::runtime_error("internal error : invalid row index");

		return *_rows[row_idx].get();
	}


	Table& Table::merge(const Table& other)
	{
		if (other._rows.size() != _rows.size())
			throw std::runtime_error("internal error : table can not be merged, number of rows is different");

		// Save the number of columns in this table before adding new headers.
		const size_t previous_col_count = col_count();

		// Append headers from the other table.
		for (int col_idx = 1; col_idx < other._headers.size(); col_idx++) {
			_headers.push_back(other._headers[col_idx]);
			_wrap_positions.push_back(other._wrap_positions[col_idx]);
		}

		// Copy all rows.
		auto it_this = _rows.begin();
		auto it_other = other._rows.begin();
		for (; it_this != _rows.end() && it_other != other._rows.end(); it_this++, it_other++) {
			Row* this_row{ it_this->get() };
			this_row->resize(col_count());

			Row* other_row{ it_other->get() };
			size_t col_idx = previous_col_count;
			for (size_t idx = 1; idx < other._headers.size(); idx++, col_idx++) {
				const Cell& cell = other_row->cell(idx);
				this_row->cell(col_idx).append(cell);
			}
		}

		return *this;
	}


	Table& Table::append(const Table& other)
	{
		if (col_count() == 0) {
			init(other.headers(), other.wrap_positions());
		}
		else {
			if (other.col_count() != col_count())
				throw std::runtime_error("table can not be appended, number of columns is different");

			// add a delimiter row
			add_row();
		}

		// copy all rows from the other table
		for (auto& other_row : other._rows) {
			Row& new_row{ add_row() };

			for (size_t cidx = 0; cidx < col_count(); cidx++) {
				const Cell& cell = other_row->cell(cidx);
				new_row.cell(cidx).append(cell);
			}
		}

		return *this;
	}


	size_t Table::col_width(size_t col_idx) const
	{
		// is it a valid column index ?
		if (col_idx >= _headers.size())
			throw std::runtime_error("internal error : invalid col index");

		size_t max_width{ _headers[col_idx].size() };
		for (const auto& row : _rows)
			max_width = std::max(max_width, row->width(col_idx));

		return max_width;
	}


	Row::Row(size_t col_count) :
		_cells{ col_count }
	{
	}


	void Row::resize(size_t col_count)
	{
		_cells.resize(col_count);
	}


	Cell& Row::cell(size_t col_idx)
	{
		// is it a valid column index ?
		if (col_idx >= _cells.size())
			throw std::runtime_error("internal error : invalid col index");

		return _cells[col_idx];
	}


	const Cell& Row::cell(size_t col_idx) const
	{
		// is it a valid column index ?
		if (col_idx >= _cells.size())
			throw std::runtime_error("internal error : invalid col index");

		return _cells[col_idx];
	}


	size_t Row::height() const
	{
		size_t max_height{ 0 };

		for (int col_num = 0; col_num < _cells.size(); col_num++)
			max_height = std::max(max_height, _cells[col_num].height());

		return max_height;
	}


	size_t Row::width(size_t col_idx) const
	{
		return cell(col_idx).width();
	}


	Cell::Cell() :
		_lines{},
		_width{ 0 },
		_pending_nl{ true }
	{
	}


	Cell& Cell::append(const Cell& cell)
	{
		for (const std::string& value : cell._lines) {
			_lines.push_back(value);
			_width = std::max(_width, value.size());
		}

		return *this;
	}


	Cell& Cell::append(const std::string& value)
	{
		std::string::size_type start{ 0 };
		while (start < value.size()) {
			if (_pending_nl) {
				_lines.push_back("");
				_pending_nl = false;
			}

			const std::string::size_type end{ value.find('\n', start) };
			if (end != std::string::npos) {
				// Append the string delimited by \n
				_lines.back().append(value.substr(start, end - start));

				// Don't forget to append a new empty string when a new line
				// is appended to this cell.
				_pending_nl = true;

				// Consider the rest of the given line.
				start = end + 1;
			}
			else {
				_lines.back().append(value.substr(start));
				start = end;
			}
		}

		if (_lines.size() > 0)
			_width = std::max(_width, _lines.back().size());

		return *this;
	}


	Cell& Cell::append(const int value)
	{
		return append(std::to_string(value));
	}


	Cell& Cell::append(const size_t value)
	{
		return append(std::to_string(value));
	}


	Cell& Cell::append(const std::vector<int>& values)
	{
		int idx = 0;

		for (const int value : values) {
			append(value);
			if (idx < values.size() - 1)
				append(", ");

			idx++;
		}

		return *this;
	}


	Cell& Cell::append(const std::vector<std::string>& values)
	{
		int idx = 0;

		for (const std::string& value : values) {
			append(value);
			if (idx < values.size() - 1)
				append(", ");

			idx++;
		}

		return *this;
	}


	Cell& Cell::append_nl()
	{
		return append("\n");
	}


	Cell& Cell::append_nl(const std::string& value)
	{
		return append(value).append_nl();
	}


	const std::string& Cell::line(size_t line_idx) const
	{
		static std::string empty{};

		if (line_idx < _lines.size())
			return _lines[line_idx];
		else
			return empty;
	}


	std::string Cell::to_string(const std::string& delim) const
	{
		return rat::strings_join(_lines, delim, false);
	}

}
