/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#include "ipv4parser.h"

#include <vector>

#include "model/ipaddress.h"
#include "model/ipconvertor.h"
#include "model/domains.h"
#include "tools/strutil.h"


namespace fwm {

	// Forward declaration
	static bool decode_network_address(const std::vector<std::string>& addr, uint32_t& network, uint32_t& mask);
	static bool decode_network_range(const std::vector<std::string>& addr, uint32_t& lbound, uint32_t& ubound);


	std::tuple<IPAddressType, uint32_t, uint32_t> decode_ipv4_network_address(const std::string& addr, bool strict)
	{
		std::vector<std::string> parts;
		IPAddressType at;
		uint32_t address_lower;
		uint32_t address_upper;

		if (addr.find('/') != std::string::npos) {
			// Split the network and the mask
			if (rat::split(addr, '/', parts) != 2)
				throw IPAddressError(addr, "format error, multiple /");

			uint32_t network;
			uint32_t mask;
			if (!decode_network_address(parts, network, mask))
				throw IPAddressError(addr, "invalid format");

			if (strict && ((network & mask) != network))
				throw IPAddressError(addr, "host bits set");

			address_lower = network;
			address_upper = network + ~mask;

			if (address_upper < address_lower)
				throw IPAddressError(addr, "invalid address");

			at = address_upper > address_lower ? IPAddressType::Subnet : IPAddressType::Address;
		}
		else if (addr.find('-') != std::string::npos) {
			if (rat::split(addr, '-', parts) != 2)
				throw IPAddressError(addr, "format error, multiple -");

			address_lower = address_upper = 0;
			at = IPAddressType::Range;
			if (!decode_network_range(parts, address_lower, address_upper))
				throw IPAddressError(addr, "invalid format");
			if (address_lower > address_upper)
				throw IPAddressError(addr, "invalid range");
		}
		else {
			if (!decode_ipv4_address(addr, address_lower))
				throw IPAddressError(addr, "invalid format");

			address_upper = address_lower;
			at = IPAddressType::Address;
		}

		return std::make_tuple(at, address_lower, address_upper);
	}


	static bool decode_network_address(const std::vector<std::string>& addr, uint32_t& network, uint32_t& mask)
	{
		if (!decode_ipv4_address(addr[0], network))
			return false;

		if (addr.size() == 2) {
			if (addr[1].size() <= 2) {
				long nbits;
				if (!rat::str2num(addr[1], 10, 0, 32, nbits))
					return false;

				mask = UINT32_MAX;
				if (nbits < 32)
					mask = ~(mask >> nbits);
			}
			else
				return decode_ipv4_address(addr[1], mask);
		}
		else {
			mask = INT32_MAX;
		}

		return true;
	}


	static bool decode_network_range(const std::vector<std::string>& addr, uint32_t& lbound, uint32_t& ubound)
	{
		if (!decode_ipv4_address(addr[0], lbound))
			return false;

		if (addr.size() == 2) {
			if (!decode_ipv4_address(addr[1], ubound))
				return false;
		}
		else {
			ubound = lbound;
		}

		return true;
	}


	bool is_ipv4_network_address(const std::string& addr, bool strict)
	{
		bool valid{ true };
		try {
			decode_ipv4_network_address(addr, strict);
		}
		catch (const IPAddressError&) {
			valid = false;
		}

		return valid;
	}

}
