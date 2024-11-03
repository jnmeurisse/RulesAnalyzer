/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#include "policyreader.h"

#include <cstdio>

#include "tools/csvreader.h"
#include "tools/logger.h"
#include "tools/strutil.h"


namespace fos {

	static std::vector<csv::CsvColumn> columns{
		{"id",         "",        false},
		{"name",       "",        true},
		{"status",     "enabled", true},
		{"action",     "",        false},
		{"src.zone",   "any",     true},
		{"src.addr",   "",        false},
		{"src.negate", "false",   true},
		{"dst.zone",   "any",     true},
		{"dst.addr",   "",        false},
		{"dst.negate", "false",   true },
		{"svc",        "",        false},
		{"app",        "any",     true},
		{"user",       "any",     true},
		{"url",        "any",     true}
	};


	PolicyReader::PolicyReader(std::istream& csv_stream, const CsvReaderConfig& reader_config, f_interrupt_cb interrupt_cb) :
		_reader{ csv_stream, columns, interrupt_cb },
		_reader_config{ reader_config },
		_logger{  Logger::get_logger() }
	{
	}


	bool fos::PolicyReader::next_row(RuleObject& rule)
	{
		rule.clear();

		csv::CsvValues values;
		if (!_reader.next_row(values))
			return false;

		bool valid{ true };
		int value_index{ 0 };
		rule.row_number = _reader.row_number();
		if (!rat::str2i(values[value_index++], rule.id) || rule.id <= 0) {
			_logger->error("row %d : %s is not a valid rule id",
				_reader.row_number(),
				values[value_index - 1].c_str()
			);

			valid = false;
		}

		rule.name = values[value_index++];
		rule.status = values[value_index++];
		rule.action = values[value_index++];

		const char csv_list_delimiter = _reader_config.csv_list_delimiter;
		if (rat::split(values[value_index++], csv_list_delimiter, rule.source_zones) == 0) {
			_logger->error("row %d : source zones is empty", _reader.row_number());
			valid = false;
		}

		if (rat::split(values[value_index++], csv_list_delimiter, rule.source_addresses) == 0) {
			_logger->error("row %d : source addresses is empty", _reader.row_number());
			valid = false;
		}

		if (!rat::str2b(values[value_index++], rule.negate_source_addresses)) {
			_logger->error("row %d : negate source addresses is not a boolean", _reader.row_number());
			valid = false;
		}

		if (rat::split(values[value_index++], csv_list_delimiter, rule.destination_zones) == 0) {
			_logger->error("row %d : destination zones is empty", _reader.row_number());
			valid = false;
		}

		if (rat::split(values[value_index++], csv_list_delimiter, rule.destination_addresses) == 0)  {
			_logger->error("row %d : destination addresses is empty", _reader.row_number());
			valid = false;
		}

		if (!rat::str2b(values[value_index++], rule.negate_destination_addresses)) {
			_logger->error("row %d : negate destination addresses is not a boolean", _reader.row_number());
			valid = false;
		}

		if (rat::split(values[value_index++], csv_list_delimiter, rule.services) == 0) {
			_logger->error("row %d : services is empty", _reader.row_number());
			valid = false;
		}

		if (rat::split(values[value_index++], csv_list_delimiter, rule.applications) == 0) {
			_logger->error("row %d : applications is empty", _reader.row_number());
			valid = false;
		}

		if (rat::split(values[value_index++], csv_list_delimiter, rule.users) == 0) {
			_logger->error("row %d : users is empty", _reader.row_number());
			valid = false;
		}

		if (rat::split(values[value_index++], csv_list_delimiter, rule.urls) == 0) {
			_logger->error("row %d : urls is empty", _reader.row_number());
			valid = false;
		}

		return valid;
	}
}
