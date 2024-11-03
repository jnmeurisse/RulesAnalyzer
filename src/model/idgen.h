/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#pragma once
#include "global.h"

#include <cstdint>
#include <string>
#include <map>

namespace fwm {

	/* A Unique ID generator.
	 *
	 * An IdGenerator is a table that stores strings and corresponding identifiers.
	 * The generator creates sequential id starting from 1 up to 'ubound'.
	*/
	class IdGenerator {
	public:
		explicit IdGenerator(const std::string& name, uint32_t ubound);

		/* Returns an id for the given string.
		 *
		 * Adds a string to the id table and returns a unique identifier identifying
		 * the string.  If the string is already in the table, the function returns
		 * the associated identifier.
		*/
		uint32_t get_id(const std::string& str);

	private:
		// The name of this id generator.
		const std::string _name;

		// The largest id we can allocate.
		const uint32_t _ubound;

		// The next identifier
		uint32_t _next_id;

		// string to id association table.
		std::map<std::string, uint32_t> _id_table;
	};

}
