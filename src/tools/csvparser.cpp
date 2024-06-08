/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#include "csvparser.h"

namespace csv {

	CsvParser::CsvParser(std::istream& csv_stream, const CsvParserOptions& options) :
		_state{ ParserState::START_RECORD },
		_csv_stream{ csv_stream },
		_options{ options }
	{
	}


	bool CsvParser::next_record(CsvValues& values)
	{
		std::string field_builder;

		// set the initial state of the parser
		_state = ParserState::START_RECORD;

		// clear the field list and the builder.
		values.clear();
		field_builder.clear();

		// parse the stream to an end of line or file
		char c;
		do {
			_csv_stream.get(c);

			if (_csv_stream.eof()) {
				process_char(_options.endOfLineChar, field_builder, values);
				return values.size() > 0;
			}

			// skip carriage return
			if (c == 0x0D)
				continue;
		} while (!process_char(c, field_builder, values));

		return true;
	}


	bool CsvParser::process_char(char c, std::string& field_builder, CsvValues& field_list)
	{
		bool eol = false;

		switch (_state) {
		case ParserState::START_RECORD: { 
			// start of record
			if (c == _options.endOfLineChar)
				// it is an empty line
				eol = true;
			else {
				// normal character - handle as START_FIELD, recursive call to the parser
				_state = ParserState::START_FIELD;
				process_char(c, field_builder, field_list);
			}
		}
		break;

		case ParserState::START_FIELD: { 
			// expecting field
			if (c == _options.endOfLineChar) {
				// save an empty field
				field_list.push_back(field_builder);
				field_builder.clear();

				_state = ParserState::START_RECORD;
				eol = true;
			}
			else if (c == _options.quoteChar) {
				//start quoted field
				_state = ParserState::IN_QUOTED_FIELD;
			}
			else if (c == _options.escapeChar) {
				// possible escaped character
				_state = ParserState::ESCAPED_CHAR;
			}
			else if (c == _options.separatorChar) {
				// save an empty field, fieldBuilder must be empty 
				field_list.push_back(field_builder);
				field_builder.clear();
			}
			else {
				// begin a new unquoted field
				field_builder.push_back(c);
				_state = ParserState::IN_FIELD;
			}
		}
		break;

		case ParserState::ESCAPED_CHAR: {
			if (c == _options.endOfLineChar) {
				if (!_options.strictParsing) {
					field_list.push_back(field_builder);
					field_builder.clear();

					_state = ParserState::START_RECORD;
					eol = true;
				}
				else {
					throw std::runtime_error("unexpected end of line in field");
				}
			}
			else if ((c != _options.escapeChar) && (c != _options.separatorChar) && (c != _options.quoteChar)) {
				field_builder.push_back(_options.escapeChar);
			}

			field_builder.push_back(c);
			_state = ParserState::IN_FIELD;
		}
		break;

		case ParserState::IN_FIELD: {
			// in unquoted field
			if (c == _options.endOfLineChar) {
				// end of line - add the field
				field_list.push_back(field_builder);
				field_builder.clear();

				_state = ParserState::START_RECORD;
				eol = true;
			}
			else if (c == _options.escapeChar) {
				// possible escaped character * }
				_state = ParserState::ESCAPED_CHAR;
			}
			else if (c == _options.separatorChar) {
				// save field - wait for new field
				field_list.push_back(field_builder);
				field_builder.clear();
				_state = ParserState::START_FIELD;
			}
			else {
				// normal character - save in field
				field_builder.push_back(c);
			}
		}
		break;

		case ParserState::IN_QUOTED_FIELD: {
			// in quoted field
			if (c == _options.endOfLineChar) {
				if (!_options.strictParsing) {
					field_builder.push_back(c);
					eol = true;
				}
				else {
					throw std::runtime_error("unexpected end of line in field");
				}
			}
			else if (c == _options.escapeChar) {
				// Possible escape character
				_state = ParserState::ESCAPE_IN_QUOTED_FIELD;
			}
			else if (c == _options.quoteChar) {
				if (_options.msDoubleQuote) {
					// microsoft style double quotes; " represented by "" 
					_state = ParserState::QUOTE_IN_QUOTED_FIELD;
				}
				else {
					// end of quote part of field
					_state = ParserState::IN_FIELD;
				}
			}
			else {
				// normal character - save in field
				field_builder.push_back(c);
			}
		};
		break;

		case ParserState::ESCAPE_IN_QUOTED_FIELD: {
			if (c == _options.endOfLineChar) {
				if (!_options.strictParsing) {
					field_list.push_back(field_builder);
					field_builder.clear();

					_state = ParserState::START_RECORD;
					eol = true;
				}
				else {
					throw std::runtime_error("unexpected end of line in field");
				}
			}
			else if ((c != _options.escapeChar) && (c != _options.separatorChar) && (c != _options.quoteChar)) {
				field_builder.push_back(_options.escapeChar);
			}

			field_builder.push_back(c);
			_state = ParserState::IN_QUOTED_FIELD;
		};
		break;

		case ParserState::QUOTE_IN_QUOTED_FIELD: {
			// MICROSOFT double quotes - seen a quote in an quoted field
			if (c == _options.endOfLineChar) {
				field_list.push_back(field_builder);
				field_builder.clear();

				_state = ParserState::START_RECORD;
				eol = true;
			}
			else if (c == _options.quoteChar) {
				//  *save "" as "
				field_builder.push_back(c);

				_state = ParserState::IN_QUOTED_FIELD;
			}
			else if (c == _options.separatorChar) {
				//*save field - wait for new field
				field_list.push_back(field_builder);
				field_builder.clear();

				_state = ParserState::START_FIELD;
			}
			else if (!_options.strictParsing) {
				field_builder.push_back(c);
				_state = ParserState::IN_FIELD;

			}
			else {
				// illegal
				//raise EParserException.CreateFmt('unexpected character %s after %s',[c, _quoteChar]);
			};
		};
		break;	
		}

		return eol;
	}
}
