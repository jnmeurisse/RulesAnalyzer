/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#include "address.h"

#include <cassert>
#include <memory>
#include <vector>

#include "domains.h"
#include "ipv4parser.h"
#include "moptions.h"

#include "tools/strutil.h"

namespace fwm {

	Ipv4Address::Ipv4Address(const std::string& name, DomainType dt, Ipv4AddressType at, uint32_t lower, uint32_t upper) :
		NamedMnode(name , ModelOptions::empty()),
		_at{ at },
		_address{ dt, lower, upper }
	{
	}


	static bool is_power_of_2(size_t value)
	{
		return value > 0 && !(value & (value - 1));
	}


	static size_t bit_count(size_t value)
	{
		size_t count{ 0 };

		while (value) {
			if (value & 0x01)
				count++;		// bit is set
			value >>= 1;
		}

		return count;
	}


	bdd Ipv4Address::make_bdd() const
	{
		return _address.make_bdd();
	}


	const Interval& Ipv4Address::interval() const
	{
		return _address;
	}


	std::string Ipv4Address::to_string() const
	{
		std::string result;

		// Extract bytes from the address
		const uint8_t addr[4] = {
			(_address.lower() >> 24) & 0xff,
			(_address.lower() >> 16) & 0xff,
			(_address.lower() >> 8) & 0xff,
			_address.lower() & 0xff
		};

		// Get the range
		const size_t range{ _address.size() };

		// Format
		if (range == 1) {
			result = string_format(
				"%d.%d.%d.%d",
				addr[0], addr[1], addr[2], addr[3]);
		}
		else if (is_power_of_2(range)) {
			result = string_format(
				"%d.%d.%d.%d/%d",
				addr[0], addr[1], addr[2], addr[3], 32 - bit_count(range - 1));
		}
		else {
			result = string_format(
					"%d.%d.%d.%d+%zu",
					addr[0], addr[1], addr[2], addr[3], range-1);
		}

		return result;
	}


	AddressGroup::AddressGroup(const std::string& name) :
		Group<Ipv4Address>(name)
	{
	}


	bdd AddressGroup::make_bdd() const
	{
		throw std::runtime_error("internal error: make_bdd not allowed");
	}


	SrcAddress::SrcAddress(const std::string& name, Ipv4AddressType at, uint32_t lower, uint32_t upper) :
		Ipv4Address(name, DomainType::SrcAddress, at, lower, upper)
	{
	}


	SrcAddress* SrcAddress::create(const std::string& name, const std::string& addr, bool strict)
	{
		auto range{ parse_network_address(addr, strict) };
		return new SrcAddress(name, std::get<0>(range), std::get<1>(range), std::get<2>(range));
	}


	SrcAddress* SrcAddress::any()
	{
		return new SrcAddress(
			"any",
			Ipv4AddressType::Subnet,
			0,
			Domains::get(DomainType::SrcAddress).upper()
		);
	}


	SrcAddressGroup::SrcAddressGroup(const std::string& name) :
		Group<SrcAddress>(name)
	{
	}



	SrcAddressGroup::SrcAddressGroup(const std::string& name, const SrcAddress* address) :
		Group<SrcAddress>(name, address)
	{
	}


	DstAddress::DstAddress(const std::string& name, Ipv4AddressType at, uint32_t lower, uint32_t upper) :
		Ipv4Address(name, DomainType::DstAddress, at, lower, upper)
	{
	}


	DstAddress* DstAddress::create(const std::string& name, const std::string& addr, bool strict)
	{
		auto range{ parse_network_address(addr, strict) };
		return new DstAddress(name, std::get<0>(range), std::get<1>(range), std::get<2>(range));
	}


	DstAddress* DstAddress::any()
	{
		return new DstAddress(
			"any",
			Ipv4AddressType::Subnet,
			0,
			Domains::get(DomainType::DstAddress).upper()
		);
	}


	DstAddressGroup::DstAddressGroup(const std::string& name) :
		Group<DstAddress>(name)
	{
	}


	DstAddressGroup::DstAddressGroup(const std::string& name, const DstAddress* address) :
		Group<DstAddress>(name, address)
	{
	}

}
