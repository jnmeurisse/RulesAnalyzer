/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#include <vector>

#include "domains.h"
#include "ipv4parser.h"
#include "tools/strutil.h"

namespace fwm {

	Ipv4AddressError::Ipv4AddressError(const std::string& address, const std::string& reason) :
		std::runtime_error(
			string_format(
				"'%s' is not a valid IPv4 address : '%s'",
				address.c_str(),
				reason.c_str()
			)),
		_address{ address },
		_reason{ reason }
	{
	}

	// Forward declaration
	static bool parse_network(const std::vector<std::string>& addr, uint32_t& network, uint32_t& mask);
	static bool parse_cidr(const std::string& cidr, uint32_t& value);
	static bool parse_range(const std::vector<std::string>& addr, uint32_t& lower, uint32_t& upper);


	std::tuple<Ipv4AddressType, uint32_t, uint32_t> parse_network_address(const std::string& addr, bool strict)
	{
		std::vector<std::string> parts;
		Ipv4AddressType at;
		uint32_t address_lower;
		uint32_t address_upper;

		if (iequal(addr, "any")) {
			address_lower = 0;
			address_upper = Domains::get(DomainType::DstAddress).upper();
			at = Ipv4AddressType::Subnet;
		}
		else if (addr.find('/') != std::string::npos) {
			// Split the nw and the netmask
			if (split(addr, '/', parts) != 2)
				throw Ipv4AddressError(addr, "invalid network address");

			uint32_t network;
			uint32_t mask;
			if (!parse_network(parts, network, mask))
				throw Ipv4AddressError(addr, "invalid network address");

			if (strict && (network & mask) != network)
				throw Ipv4AddressError(addr, "invalid network address");

			address_lower = network;
			address_upper = network + (0xFFFFFFFF & ~mask);
			at = address_upper > address_lower ? Ipv4AddressType::Subnet : Ipv4AddressType::Address;
		}
		else if (addr.find('-') != std::string::npos) {
			if (split(addr, '-', parts) != 2)
				throw Ipv4AddressError(addr, "invalid range");

			address_lower = address_upper = 0;
			at = Ipv4AddressType::Range;
			if (!parse_range(parts, address_lower, address_upper) || (address_lower > address_upper))
				throw Ipv4AddressError(addr, "invalid range");
		}
		else {
			if (!parse_cidr(addr, address_lower))
				throw Ipv4AddressError(addr, "invalid address");

			address_upper = address_lower;
			at = Ipv4AddressType::Address;
		}

		return std::make_tuple(at, address_lower, address_upper);
	}


	static bool parse_network(const std::vector<std::string>& addr, uint32_t& network, uint32_t& mask)
	{
		if (!parse_cidr(addr[0], network))
			return false;

		if (addr.size() == 2) {
			if (addr[1].size() <= 2) {
				long nbits;
				if (!str2num(addr[1], 10, 0, 32, nbits))
					return false;

				const uint64_t all_one = 0xFFFFFFFF;
				mask = all_one ^ (all_one >> (nbits));
			}
			else
				return parse_cidr(addr[1], mask);
		}
		else {
			mask = 0xFFFFFFFF;
		}

		return true;
	}


	static bool parse_range(const std::vector<std::string>& addr, uint32_t& lower, uint32_t& upper)
	{
		if (!parse_cidr(addr[0], lower))
			return false;

		if (addr.size() == 2) {
			if (!parse_cidr(addr[1], upper))
				return false;
		}
		else {
			upper = lower;
		}

		return true;
	}


	static bool parse_cidr(const std::string& cidr, uint32_t& value)
	{
		value = 0;
		std::vector<std::string> octets;

		if (split(cidr, '.', octets) != 4)
			return false;

		for (int i = 0; i < octets.size(); i++) {
			long octet;
			if (!str2num(octets[i], 10, 0, 255, octet))
				return false;

			value = (value << 8) | octet;
		}

		return true;
	}


	bool is_network_address(const std::string& addr, bool strict)
	{
		bool valid{ true };
		try {
			parse_network_address(addr, strict);
		}
		catch (const Ipv4AddressError&) {
			valid = false;
		}

		return valid;
	}

}
