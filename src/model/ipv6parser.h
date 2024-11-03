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
#include <tuple>

#include "model/ipaddress.h"
#include "tools/uint128.h"


namespace fwm {

	/*
	* Decode an IPv6 address.
	*
	* The function accepts the following syntaxes :
	*  A sub network
	*  A single address
	*  An address range
	*
	* The method returns a tuple (type, start ip address, end ip address).
	*/
	std::tuple<IPAddressType, uint128_t, uint128_t> decode_ipv6_network_address(const std::string& addr, bool strict);


	/* Returns true if the string is a valid ip address or ip address range.
	*/
	bool is_ipv6_network_address(const std::string& addr, bool strict);

}
