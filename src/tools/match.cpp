/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#include "tools/match.h"

#include <string>
#include <ctype.h>

namespace rat {

	bool match(const std::string& pattern, const std::string& text)
	{
		// code from https://www.geeksforgeeks.org/wildcard-pattern-matching/
		size_t n = text.length();
		size_t m = pattern.length();
		int i = 0, j = 0, startIndex = -1, match = 0;

		while (i < n)
		{
			if (j < m && (pattern[j] == '?' || tolower(pattern[j]) == tolower(text[i])))
			{
				// Characters match or '?' in pattern matches any character.
				i++;
				j++;
			}
			else if (j < m && pattern[j] == '*')
			{
				// Wildcard character '*',
				// mark the current position in the pattern and the text as a proper match.
				startIndex = j;
				match = i;
				j++;
			}
			else if (startIndex != -1)
			{
				// No match, but a previous wildcard was found.
				// Backtrack to the last '*' character position and try for a different match.
				j = startIndex + 1;
				match++;
				i = match;
			}
			else
			{
				// If none of the above cases comply, the pattern does not match.
				return false;
			}
		}

		// Consume any remaining '*' characters in the given pattern.
		while (j < m && pattern[j] == '*')
		{
			j++;
		}

		// If we have reached the end of both the pattern and the text,
		// the pattern matches the text.
		return j == m;
	}

}
