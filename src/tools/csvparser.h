/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#pragma once
#include <exception>
#include <iostream>
#include <list>
#include <string>
#include <tuple>
#include <vector>

namespace csv {
	using CsvValues = std::vector<std::string>;

	class CsvParserEofError : public std::runtime_error
	{
	public:
		CsvParserEofError(const std::string& message) :
			std::runtime_error(message)
		{};
	};


	struct CsvParserOptions {
		char endOfLineChar;
		char quoteChar;
		char escapeChar;
		char separatorChar;
		bool msDoubleQuote;
		bool strictParsing;
	};

	class CsvParser {
	public:
		CsvParser(std::istream& csv_stream, const CsvParserOptions& options);

		bool next_record(CsvValues& values);

	private:
		const CsvParserOptions _options;
		std::istream& _csv_stream;

		enum class ParserState {
			START_RECORD, START_FIELD, ESCAPED_CHAR,
			IN_FIELD, IN_QUOTED_FIELD, ESCAPE_IN_QUOTED_FIELD,
			QUOTE_IN_QUOTED_FIELD
		};

		/*
		* this is the current state of the parser. The process_char
		* method uses and changes this state according to characters
		* read by the reader.
		*/
		ParserState _state;

		bool process_char(char c, std::string& field_builder, CsvValues& field_list);
	};
}
