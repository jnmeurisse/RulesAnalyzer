/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#include "tools/csvparser.h"
#include "fmt/core.h"


namespace csv {
	static const csv::CsvParserOptions default_parser_option{ '"', '\\', ' ', false, true };


	CsvParser::CsvParser(std::istream& csv_stream) :
		CsvParser(csv_stream, default_parser_option)
	{

	}


	CsvParser::CsvParser(std::istream& csv_stream, const CsvParserOptions& options) :
		_csv_stream{ csv_stream },
		_options{ options }
	{
	}


	bool CsvParser::next_record(CsvValues& values)
	{
		std::string field_builder;

		// set the initial state of the parser
		ParserState state = ParserState::START_RECORD;

		// clear the field list and the builder.
		values.clear();
		field_builder.clear();

		// parse the stream to an end of line or file
		char c;
		bool cr_detected = false;
		bool eol = false;

		do {
			_csv_stream.get(c);

			if (_csv_stream.eof()) {
				if (state != ParserState::START_RECORD)
					process_char('\n', state, field_builder, values);
				eol = true;
			}
			else {
				if (cr_detected) {
					if (c == '\r') {
						eol = process_char('\r', state, field_builder, values);
					}
					else if (c == '\n') {
						eol = process_char('\n', state, field_builder, values);
						cr_detected = false;
					}
					else
						throw std::runtime_error("csv : invalid end of line characters");
				}
				else if (c == '\r')
					cr_detected = true;
				else
					eol = process_char(c, state, field_builder, values);
			}
		} while (!eol);

		return values.size() > 0;
	}


	bool CsvParser::process_char(char c, ParserState& state, std::string& field_builder, CsvValues& field_list)
	{
		bool eol = false;

		switch (state) {
		case ParserState::START_RECORD: {
			// start of record
			if (c == '\n')
				// it is an empty line
				eol = true;
			else {
				// normal character - handle as START_FIELD, recursive call to the parser
				state = ParserState::START_FIELD;
				process_char(c, state, field_builder, field_list);
			}
		}
		break;

		case ParserState::START_FIELD: {
			// expecting field
			if (c == '\n') {
				// save an empty field
				field_list.push_back(field_builder);
				field_builder.clear();

				state = ParserState::START_RECORD;
				eol = true;
			}
			else if (c == _options.quoteChar) {
				//start quoted field
				state = ParserState::IN_QUOTED_FIELD;
			}
			else if (c == _options.escapeChar) {
				// possible escaped character
				state = ParserState::ESCAPED_CHAR;
			}
			else if (c == _options.separatorChar) {
				// save an empty field, fieldBuilder must be empty
				field_list.push_back(field_builder);
				field_builder.clear();
			}
			else {
				// begin a new unquoted field
				field_builder.push_back(c);
				state = ParserState::IN_FIELD;
			}
		}
		break;

		case ParserState::ESCAPED_CHAR: {
			if (c == '\n') {
				if (!_options.strictParsing) {
					field_list.push_back(field_builder);
					field_builder.clear();

					state = ParserState::START_RECORD;
					eol = true;
				}
				else {
					throw std::runtime_error("csv : unexpected end of line in field");
				}
			}
			else if ((c != _options.escapeChar) && (c != _options.separatorChar) && (c != _options.quoteChar)) {
				field_builder.push_back(_options.escapeChar);
			}

			field_builder.push_back(c);
			state = ParserState::IN_FIELD;
		}
		break;

		case ParserState::IN_FIELD: {
			// in unquoted field
			if (c == '\n') {
				// end of line - add the field
				field_list.push_back(field_builder);
				field_builder.clear();

				state = ParserState::START_RECORD;
				eol = true;
			}
			else if (c == _options.escapeChar) {
				// possible escaped character * }
				state = ParserState::ESCAPED_CHAR;
			}
			else if (c == _options.separatorChar) {
				// save field - wait for new field
				field_list.push_back(field_builder);
				field_builder.clear();
				state = ParserState::START_FIELD;
			}
			else {
				// normal character - save in field
				field_builder.push_back(c);
			}
		}
		break;

		case ParserState::IN_QUOTED_FIELD: {
			// in quoted field
			if (c == '\n') {
				if (!_options.strictParsing) {
					field_builder.push_back(c);
					eol = true;
				}
				else {
					throw std::runtime_error("csv : unexpected end of line in field");
				}
			}
			else if (c == _options.escapeChar) {
				// Possible escape character
				state = ParserState::ESCAPE_IN_QUOTED_FIELD;
			}
			else if (c == _options.quoteChar) {
				if (_options.msDoubleQuote) {
					// microsoft style double quotes; " represented by ""
					state = ParserState::QUOTE_IN_QUOTED_FIELD;
				}
				else {
					// end of quote part of field
					state = ParserState::IN_FIELD;
				}
			}
			else {
				// normal character - save in field
				field_builder.push_back(c);
			}
		};
		break;

		case ParserState::ESCAPE_IN_QUOTED_FIELD: {
			if (c == '\n') {
				if (!_options.strictParsing) {
					field_list.push_back(field_builder);
					field_builder.clear();

					state = ParserState::START_RECORD;
					eol = true;
				}
				else {
					throw std::runtime_error("csv : unexpected end of line in field");
				}
			}
			else if ((c != _options.escapeChar) && (c != _options.separatorChar) && (c != _options.quoteChar)) {
				field_builder.push_back(_options.escapeChar);
			}

			field_builder.push_back(c);
			state = ParserState::IN_QUOTED_FIELD;
		};
		break;

		case ParserState::QUOTE_IN_QUOTED_FIELD: {
			// MICROSOFT double quotes - seen a quote in an quoted field
			if (c == '\n') {
				field_list.push_back(field_builder);
				field_builder.clear();

				state = ParserState::START_RECORD;
				eol = true;
			}
			else if (c == _options.quoteChar) {
				//  save "" as "
				field_builder.push_back(c);

				state = ParserState::IN_QUOTED_FIELD;
			}
			else if (c == _options.separatorChar) {
				// save field - wait for new field
				field_list.push_back(field_builder);
				field_builder.clear();

				state = ParserState::START_FIELD;
			}
			else if (!_options.strictParsing) {
				field_builder.push_back(c);
				state = ParserState::IN_FIELD;

			}
			else {
				// illegal
				throw std::runtime_error(
					fmt::format("unexpected character {} after {}", c, _options.quoteChar)
				);
			};
		};
		break;
		}

		return eol;
	}
}
