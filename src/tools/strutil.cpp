/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#include "tools/strutil.h"

#include <algorithm>
#include <cctype>
#include <sstream>
#include <limits>
#include <stdarg.h>
#include <stdexcept>
#include <stdio.h>
#include <vector>


namespace rat {

	int split(const std::string& str, const char delim, std::vector<std::string>& parts)
	{
		const size_t count = parts.size();
		size_t start{ 0 };
		size_t end{ 0 };

		while ((start = str.find_first_not_of(delim, end)) != std::string::npos) {
			end = str.find(delim, start);
			std::string value = trim(str.substr(start, end - start));
			if (value.size() > 0)
				parts.push_back(str.substr(start, end - start));
		}

		return (int)(parts.size() - count);
	}



	std::vector<std::string> split(const std::string &str, const char delim)
	{
		std::vector<std::string> parts;
		split(str, delim, parts);

		return parts;
	}



	bool str2num(const std::string& numstr, const int radix, const long minval, const long maxval, long &value)
	{
		if (minval > maxval) {
			errno = EINVAL;

		} else {
			char* end_ptr;
			errno = 0;

			long l = std::strtol(numstr.c_str(), &end_ptr, radix);
			if (errno == 0) {
				if (*end_ptr != '\0') {
					errno = EINVAL;
				}
				else if (l < minval || l > maxval) {
					errno = ERANGE;
				}
				else {
					value = l;
				}
			}
		}

		return errno == 0;
	}


	bool str2i(const std::string& str, int& value)
	{
		long tmp;
		bool ok;

		if ((ok = str2num(str, 10, 0, std::numeric_limits<long>::max(), tmp))) {
			value = tmp;
		}

		return ok;
	}


	bool str2b(const std::string& boolstr, bool& value)
	{
		bool ok;

		if (iequal(boolstr, "true") || boolstr == "1") {
			value = true;
			ok = true;
		}
		else if (iequal(boolstr, "false") || boolstr == "0") {
			value = false;
			ok = true;
		}
		else
			ok = false;

		return ok;
	}


	static bool icheq(unsigned char a, unsigned char b)
	{
		return std::tolower(a) == std::tolower(b);
	}


	bool iequal(std::string const& s1, std::string const& s2)
	{
		return (s1.length() == s2.length()) && std::equal(s2.begin(), s2.end(), s1.begin(), icheq);
	}


	bool ends_with(const std::string& str, const std::string& ending)
	{
		if (ending.size() > str.size())
			return false;
		return std::equal(ending.rbegin(), ending.rend(), str.rbegin());
	}


	bool iends_with(const std::string& str, const std::string& ending)
	{
		if (ending.size() > str.size())
			return false;
		return std::equal(ending.rbegin(), ending.rend(), str.rbegin(), icheq);
	}


	std::string trimright(const std::string& str)
	{
		return str.length() == 0 ? str : str.substr(0, str.find_last_not_of(" \t") + 1);
	}


	std::string trimleft(const std::string& str)
	{
		return str.length() == 0 ? str : str.substr(str.find_first_not_of(" \t"));
	}


	std::string trim(const std::string & str)
	{
		return str.length() == 0 ? str : trimleft(trimright(str));
	}


	std::string tolower(std::string str)
	{
		std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c){ return std::tolower(c); });

		return str;
	}


	std::string toupper(std::string str)
	{
		std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c){ return std::toupper(c); });

		return str;
	}


	std::string quote(const std::string& str)
	{
		std::ostringstream quoted_string;

		quoted_string << '"';
		for (const char c : str)
			if (c == '"')
				quoted_string << '"' << '"';
			else
				quoted_string << c;
		quoted_string << '"';

		return quoted_string.str();
	}


	std::string strings_join(const std::vector<std::string>& strings, const std::string& delim, bool quoted)
	{
		std::ostringstream joined_string;

		for (const std::string& str : strings)
			joined_string << (quoted ? quote(str) : str) << delim;

		std::string output{ joined_string.str() };
		return output.size() > delim.size() ? output.substr(0, output.size() - delim.size()) : "";
	}


	std::string pluralize(size_t n, const std::string& singular, const std::string& plural_suffix)
	{
		return (n <= 1) ? singular : singular + plural_suffix;
	}

}
