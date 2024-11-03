/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#include "ostore/userobject.h"

namespace fos {

	UserObject::UserObject(const std::string& name) :
		FirewallObject(name)
	{
	}


	std::string UserObject::to_string() const
	{
		return name();
	}

}
