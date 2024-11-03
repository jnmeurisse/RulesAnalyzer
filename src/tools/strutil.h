/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#pragma once
#include "global.h"

#include <string>
#include <vector>
#include <string.h>


#if defined(RA_OS_WINDOWS)
   #define strcasecmp _stricmp
#endif

namespace rat {

	struct stri_comparator {
		bool operator() (const std::string& lhs, const std::string& rhs) const {
			return strcasecmp(lhs.c_str(), rhs.c_str()) < 0;
		}
	};


	// Splits a string into multiple parts which are separated by a delimiter. The function
	// adds the parts to the specified vector and returns the number of added parts.
	int split(const std::string& str, const char delim, std::vector<std::string>& parts);
	std::vector<std::string> split(const std::string& str, const char delim);

	// Performs a case insensitive string comparison
	bool iequal(std::string const& s1, std::string const& s2);

	// Checks if a string ends with another string
	bool ends_with(const std::string& str, const std::string& ending);
	bool iends_with(const std::string& str, const std::string& ending);

	// Converts a string to an integer. The function returns true if the conversion
	// succeeds. The value parameter remains untouched if an error was detected.
	bool str2num(const std::string& numstr, const int radix, const long minval, const long maxval, long& value);
	bool str2i(const std::string& numstr, int& value);

	// Converts a string to a bool. The function returns true if the conversion
	// succeeds. The value parameter remains untouched if an error was detected.
	bool str2b(const std::string& boolstr, bool& value);

	// Trims string
	std::string trimright(const std::string& str);
	std::string trimleft(const std::string& str);
	std::string trim(const std::string& str);

	// Converts string to lower case
	std::string tolower(std::string str);

	// Converts string to upper case
	std::string toupper(std::string str);

	// Quotes a string
	std::string quote(const std::string& str);

	// Joins a list of strings
	std::string strings_join(const std::vector<std::string>& strings, const std::string& delim, bool quoted);

	// Pluralizes a string
	std::string pluralize(size_t n, const std::string& singular, const std::string& plural_suffix = "s");

}
