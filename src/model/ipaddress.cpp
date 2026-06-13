/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#include "model/ipaddress.h"

#include "model/ipv4parser.h"
#include "model/ipv6parser.h"
#include "fmt/core.h"


namespace fwm {

	std::string to_string(IPAddressModel address_model)
	{
		switch (address_model)
		{
		case fwm::IPAddressModel::IP4Model:
			return "IPv4";

		case fwm::IPAddressModel::IP6Model:
			return "IPv6";

		default:
			return "IPv6+IPv4";
		}
	}


	IPAddressType get_ip_address_type(const std::string& addr, IPAddressModel ip_model, bool strict)
	{
		switch (ip_model) {
		case IPAddressModel::IP4Model:
			return std::get<0>(decode_ipv4_network_address(addr, strict));

		case IPAddressModel::IP6Model:
			if (strict)
				return std::get<0>(decode_ipv6_network_address(addr, strict));

			// When strict parsing is disabled, we allow IPv6 and IPv4 format
			// [[falltrough]]

		default:
			if (is_ipv6_network_address(addr, strict))
				return std::get<0>(decode_ipv6_network_address(addr, strict));
			else
				return std::get<0>(decode_ipv4_network_address(addr, strict));
		}
	}




	IPAddressError::IPAddressError(const std::string& address, const std::string& reason) :
		std::runtime_error(
			fmt::format("'{}' is not a valid IP address : '{}'", address, reason)
		),
		_address{ address },
		_reason{ reason }
	{
	}


	IpAddress::IpAddress(const std::string& name, DomainType dt, const Range* range) :
		NamedMnode(name, ModelOptions::empty()),
		_address_value{ std::make_unique<Mvalue>(dt, range) }
	{
	}


	IPAddressType IpAddress::at() const
	{
		const Range& range = _address_value->range();
		if (range.is_singleton())
			return IPAddressType::Address;
		else if (range.is_power_of_2())
			return IPAddressType::Subnet;
		else
			return IPAddressType::Range;
	}


	int IpAddress::version() const
	{
		switch (_address_value->range().nbits())
		{
		case 32: return 4;
		case 128: return 6;
		default:
			throw std::invalid_argument("internal error : nbits must be 32 or 128");
		}
	}


	bool IpAddress::is_valid_ip(const std::string& addr, IPAddressModel ip_model, bool strict)
	{
		switch (ip_model) {
		case IPAddressModel::IP4Model:
			return is_ipv4_network_address(addr, strict);

		case IPAddressModel::IP6Model:
			if (strict)
				return is_ipv6_network_address(addr, strict);
			// When strict parsing is disabled, we allow IPv6 and IPv4 format
			// [[falltrough]]

		default:
			return is_ipv6_network_address(addr, strict) || is_ipv4_network_address(addr, strict);
		}
	}


	bdd IpAddress::make_bdd() const
	{
		return _address_value->make_bdd();
	}


	const Mvalue& IpAddress::value() const
	{
		return *_address_value;
	}


	std::string IpAddress::to_string() const
	{
		return _address_value->to_string();
	}




}
