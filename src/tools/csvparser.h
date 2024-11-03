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
#include <list>
#include <stdexcept>
#include <string>
#include <tuple>
#include <vector>


namespace csv {
	class CsvValues : public std::vector<std::string>
	{
	public:
		CsvValues() = default;
	};


	class CsvParserEofError : public std::runtime_error
	{
	public:
		CsvParserEofError(const std::string& message) :
			std::runtime_error(message)
		{};
	};


	struct CsvParserOptions {
		char quoteChar;
		char escapeChar;
		char separatorChar;
		bool msDoubleQuote;
		bool strictParsing;
	};


	class CsvParser {
	public:
		CsvParser(std::istream& csv_stream);
		CsvParser(std::istream& csv_stream, const CsvParserOptions& options);

		bool next_record(CsvValues& values);

	private:
		const CsvParserOptions _options;
		std::istream& _csv_stream;

		/*
		* this is the possible states of the parser. The process_char
		* method uses and changes the state according to characters
		* read by the reader.
		*/
		enum class ParserState {
			START_RECORD,
			START_FIELD,
			ESCAPED_CHAR,
			IN_FIELD,
			IN_QUOTED_FIELD,
			ESCAPE_IN_QUOTED_FIELD,
			QUOTE_IN_QUOTED_FIELD
		};

		bool process_char(char c, ParserState& state, std::string& field_builder, CsvValues& field_list);
	};
}
