/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#pragma once
#include "global.h"

#include <list>
#include <memory>
#include <string>
#include <vector>

#include "tools/interrupt.h"


namespace fwm {

	/**
	* A cell is a multi line strings inside a Table
	*/
	class Cell
	{
	public:
		Cell();

		/* Misc methods that append a value to this cell.
		*/
		Cell& append(const Cell& cell);
		Cell& append(const std::string& value);
		Cell& append(const int value);
		Cell& append(const size_t value);
		Cell& append(const std::vector<int>& values);
		Cell& append(const std::vector<std::string>& values);

		/* Appends a new line to this cell.  The new line is effectively
		   added when a new string is appended.
		*/
		Cell& append_nl();

		/* Appends a text followed by a new line.
		*/
		Cell& append_nl(const std::string& value);

		/* Returns the content of the 'line_idx' line from this cell.  The methods
		   returns an empty string if the line_idx is greater than the number
		   of lines in this cell.
		*/
		const std::string& line(size_t line_idx) const;

		/* Returns a concatenation of all lines.  The lines are joined with
		   the given delimiter.
		*/
		std::string to_string(const std::string& delim) const;

		/* Returns the number of lines in this cell.
		*/
		inline size_t height() const noexcept { return _lines.size(); }

		/* Returns the maximum number of characters in each line in this cell.
		*/
		inline size_t width() const noexcept { return _width; }

	private:
		// All lines
		std::vector<std::string> _lines;

		// Current maximum number of characters in each line of the cell.
		size_t _width;

		// True when a new line must be added.
		bool _pending_nl;
	};


	/** A row in a table.
	*/
	class Row
	{
	public:
		/* Allocates a new row having 'col_count' columns.
		*/
		Row(size_t col_count);

		/* Resizes this row to contain 'col_count' columns.
		*/
		void resize(size_t col_count);

		/* Returns a reference to the cell in column 'col_idx'.  The method raises an
		   exception if the cell does not exist in this row.
		*/
		Cell& cell(size_t col_idx);
		const Cell& cell(size_t col_idx) const;

		/* Returns the maximum number of lines in this row.
		*/
		size_t height() const;

		/* Returns the maximum number of character in column 'col_idx'.
		*/
		size_t width(size_t col_idx) const;

	private:
		// All cells within this row.
		std::vector<Cell> _cells;
	};


	class Table
	{
	public:
		using Headers = std::vector<std::string>;
		using WrapPositions = std::vector<size_t>;

		/* create a table header.
		 *
		 * @param col0 The header of the first column
		 * @param cols The headers of additional columns.
		*/
		static Headers create_headers(const std::string& col0, const std::vector<std::string>& cols);

		/* Table constructors
		*/
		Table() = default;
		Table(const Headers& headers, const WrapPositions& wrap_positions);
		Table(const Headers& headers);

		/* Initializes the table.
		*/
		Table& init(const Headers& headers, const WrapPositions& wrap_positions);

		/* Clears the content of this table
		*/
		Table& clear();

		/* Adds a new empty row to this table.
		*/
		Row& add_row();

		/* Returns a reference to a row in this table.
		   The method raise an exception in the row number does not
		   exist in this table.
		*/
		Row& get_row(size_t row_idx);
		const Row& get_row(size_t row_idx) const;

		/* Merges the other table with this table.  The columns of
		   the other table are added at the right of this table.  The
		   first column from the other table is not copied.
		   The method raise an exception if the number of rows are
		   different. The methods returns a reference to this table.
		*/
		Table& merge(const Table& other);

		/* Appends the other table to this table.  The rows of
		   the other table are added at the end of this table. The
		   header from the other table is not copied.
		   The method raise an exception if the number of columns are
		   different. The methods returns a reference to this table.
		*/
		Table& append(const Table& other);

		/* Returns the table headers.
		*/
		inline const Headers& headers() const noexcept { return _headers; }

		/* Returns the number of rows in this table.
		*/
		inline size_t row_count() const noexcept { return _rows.size(); };

		/* Returns the number of columns in this table.
		*/
		inline size_t col_count() const noexcept { return _headers.size(); }

		/* Returns the maximum number of characters in the column 'col_idx'
		*/
		size_t col_width(size_t col_idx) const;

		/* Returns the position where text wraps in column.
		*/
		inline const WrapPositions& wrap_positions() const noexcept { return _wrap_positions; }

	private:
		// The table headers
		Headers _headers;

		// Positions where the text must be wrapped
		WrapPositions _wrap_positions;

		// All rows
		std::vector<std::unique_ptr<Row>> _rows;
	};

}
