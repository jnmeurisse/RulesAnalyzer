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

#include "tools/uint128.h"


namespace fwm {

	/* Converts the address text into an IPv4 address.
	*
	* @param address_text : the IPv4 address to convert.
	* @param address : a reference to a buffer that will hold the converted address.
	* @return : returns true on success.
	*
	*/
	bool decode_ipv4_address(const std::string& address_text, uint32_t& address);


	/* Converts an IPv4 address to the address text.  The function returns False
	*  if the conversion fails.
	*/
	bool encode_ipv4_address(uint32_t address, std::string& address_text);


	/* Converts the address text into an IPv6 address.
	*
	* @param address_text : the IPv6 address to convert.
	* @param address : a reference to a buffer that will hold the converted address.
	* @return : returns true on success.
	*
	*/
	bool decode_ipv6_address(const std::string& address_text, uint128_t& address);


	/* Converts an IPv4 address to the address text.  The function returns False
	*  if the conversion fails.
	*/
	bool encode_ipv6_address(uint128_t address, std::string& address_text);

}
