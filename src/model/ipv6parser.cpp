/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#include "ipv6parser.h"

#include <vector>

#include "model/domains.h"
#include "model/ipaddress.h"
#include "model/ipconvertor.h"
#include "tools/strutil.h"
#include "tools/uint128.h"


namespace fwm {

	// Forward declaration
	static bool decode_network_address(const std::vector<std::string>& addr, uint128_t& network, uint128_t& mask);
	static bool decode_network_range(const std::vector<std::string>& addr, uint128_t& lbound, uint128_t& ubound);


	std::tuple<IPAddressType, uint128_t, uint128_t> decode_ipv6_network_address(const std::string& addr, bool strict)
	{
		std::vector<std::string> parts;
		IPAddressType at;
		uint128_t address_lower;
		uint128_t address_upper;

		if (addr.find('/') != std::string::npos) {
			// Split the network and the mask
			if (rat::split(addr, '/', parts) != 2)
				throw IPAddressError(addr, "format error, multiple /");

			uint128_t network;
			uint128_t mask;
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
			if (!decode_ipv6_address(addr, address_lower))
				throw IPAddressError(addr, "invalid format");

			address_upper = address_lower;
			at = IPAddressType::Address;
		}

		return std::make_tuple(at, address_lower, address_upper);
	}


	static bool decode_network_address(const std::vector<std::string>& addr, uint128_t& network, uint128_t& mask)
	{
		static const uint128_t all_ones{ Address6Domain::max() };

		if (!decode_ipv6_address(addr[0], network))
			return false;

		if (addr.size() == 2) {
			long nbits;
			if (!rat::str2num(addr[1], 10, 0, 128, nbits))
				return false;

			mask = all_ones;
			if (nbits < 128)
				mask = ~(mask >> nbits);
		}
		else {
			mask = all_ones;
		}

		return true;
	}


	static bool decode_network_range(const std::vector<std::string>& addr, uint128_t& lbound, uint128_t& ubound)
	{
		if (!decode_ipv6_address(addr[0], lbound))
			return false;

		if (addr.size() == 2) {
			if (!decode_ipv6_address(addr[1], ubound))
				return false;
		}
		else {
			ubound = lbound;
		}

		return true;
	}


	bool is_ipv6_network_address(const std::string& addr, bool strict)
	{
		bool valid{ true };
		try {
			decode_ipv6_network_address(addr, strict);
		}
		catch (const IPAddressError&) {
			valid = false;
		}

		return valid;
	}

}
