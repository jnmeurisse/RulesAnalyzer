/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#include "ostore/addressobject.h"

#include <cassert>

#include "tools/strutil.h"


namespace fos {

	AddressObject::AddressObject(const std::string& name, const AddressType type, const std::vector<std::string>& addresses) :
		FirewallObject(name),
		_type{ type },
		_addresses{ addresses }
	{
		assert(addresses.size() >= 1 );
	}


	std::string AddressObject::to_string() const
	{
		return rat::strings_join(_addresses, ", ", false);
	}


	IpmaskAddressObject::IpmaskAddressObject(const std::string& name, const std::vector<std::string>& addresses) :
		AddressObject(name, AddressType::ipmask, addresses)
	{
	}



	IprangeAddressObject::IprangeAddressObject(const std::string& name, const std::vector<std::string>& addresses) :
		AddressObject(name, AddressType::iprange, addresses)
	{
	}


	FqdnAddressObject::FqdnAddressObject(const std::string & name, const std::string& address) :
		AddressObject(name, AddressType::fqdn, { address })
	{
	}

}
