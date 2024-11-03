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

#include "ipaddress.h"


namespace fwm {

	/*
	* Decode an IPv4 address.
	*
	* The function accepts the following syntaxes :
	*  A sub network :
	*    '192.0.2.0/24'
	*    '192.0.2.0/255.255.255.0'
	*  A single address
	*    '192.0.2.1'
	*    '192.0.2.1/255.255.255.255'
	*    '192.0.2.1/32'
	*  An address range
	*     '192.168.2.0-192.168.2.244'
	*
	* The method returns a tuple (type, start ip address, end ip address).
	*/
	std::tuple<IPAddressType, uint32_t, uint32_t> decode_ipv4_network_address(const std::string& addr, bool strict);


	/* Returns true if the string is a valid ip address or ip address range.
	*/
	bool is_ipv4_network_address(const std::string& addr, bool strict);

}
