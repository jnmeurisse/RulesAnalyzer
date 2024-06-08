/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#include "poolobject.h"

#include <stdexcept>
#include <string>

#include "tools/strutil.h"


namespace fos {

	PoolObject::PoolObject(const std::string& name, const std::vector<std::string>& members) :
		FirewallObject(name),
		_members{ members }
	{
		if (members.empty())
			throw std::runtime_error(string_format("group '%s' has no members", name.c_str()));

		for (const std::string member : members) {
			if (member.empty())
				throw std::runtime_error(string_format("group '%s' has at least one empty member", name.c_str()));
		}
	}


	std::string PoolObject::to_string() const
	{
		return strings_join(_members, ", ", false);
	}

}
