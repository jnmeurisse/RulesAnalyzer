/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#include "ostore/firewallobject.h"

#include <stdexcept>


namespace fos {

	FirewallObject::FirewallObject(const std::string& name) :
		_name{ name }
	{
		if (name.empty())
			throw std::runtime_error("firewall object name is empty");
	};


	FirewallObject::FirewallObject(const FirewallObject& other) :
		_name{ other._name }
	{
	};

}
