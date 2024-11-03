#pragma once
#include "global.h"

#include <iostream>
#include <list>
#include <string>

#include "model/table.h"
#include "tools/interrupt.h"


namespace fwm {

	class TableWriter
	{
	public:
		TableWriter(const Table& table);

	protected:
		const Table& _table;
	};

	class TableTxtWriter : public TableWriter
	{
	public:
		TableTxtWriter(const Table& table);

		/* Sets a left margin.
		*/
		TableTxtWriter& margin(int margin);

		/* Show/Hide headers.
		*/
		TableTxtWriter& header(bool visible);

		/* Writes to a stream
		*/
		void write(std::ostream& os, f_interrupt_cb interrupt_cb) const;

	private:
		int _margin;
		bool _header_visible;

		static std::string word_wrap(const std::string& text, size_t& line_length);
	};


	class TableCsvWriter : public TableWriter
	{
	public:
		TableCsvWriter(const Table& table);

		/* Sets the .csv separator symbol
		*/
		TableCsvWriter& separator(char separator);

		/* Sets the newline delimiter symbol.
		*/
		TableCsvWriter& newline(char newline);

		/* Writes to a stream
		*/
		void write(std::ostream& os, f_interrupt_cb interrupt_cb) const;

	private:
		// The .csv column separator symbol
		std::string _separator;

		// The new line separator symbol
		std::string _nl_delimiter;
	};

}
