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

#include "firewallobject.h"

namespace fos {

	/**
	* A pool is a firewall object that contains name of other firewall objects
	* of the same category.  For example, an AddressPool contains only AddressObject
	* names.  The members() method returns a reference to the object names contained in
	* this pool.
	*/
	class PoolObject abstract : public FirewallObject
	{
	public:
		PoolObject(const std::string& name, const std::vector<std::string>& members);

		/* Returns a string representation of this pool.
		*/
		virtual std::string to_string() const override;

		/* Returns the members of this pool.
		*/
		inline const std::vector<std::string>& members() const noexcept { return _members; }

	private:
		const std::vector<std::string> _members;
	};

}
