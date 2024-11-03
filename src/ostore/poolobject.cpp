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
#include "fmt/core.h"


namespace fos {

	PoolObject::PoolObject(const std::string& name, const std::vector<std::string>& members) :
		FirewallObject(name),
		_members{ members }
	{
		if (members.empty())
			throw std::runtime_error(fmt::format("group '{}' has no members", name));

		for (const std::string member : members) {
			if (member.empty())
				throw std::runtime_error(fmt::format("group '{}' has at least one empty member", name));
		}
	}


	std::string PoolObject::to_string() const
	{
		return rat::strings_join(_members, ", ", false);
	}

}
