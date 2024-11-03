/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#include "model/address.h"

#include <cassert>
#include <stdexcept>
#include <memory>
#include <vector>

#include "model/domain.h"
#include "model/domains.h"
#include "model/ipv4parser.h"
#include "model/ipv6parser.h"
#include "model/moptions.h"
#include "tools/strutil.h"


namespace fwm {

	bool is_ip_address(const std::string& addr, IPAddressModel ip_model, bool strict)
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


	IpAddress::IpAddress(const std::string& name, DomainType dt, const Range* range) :
		NamedMnode(name , ModelOptions::empty()),
		_address_value{ std::make_unique<Mvalue>(dt, range) }
	{
	}


	const IPAddressType IpAddress::at() const
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


	SrcAddress::SrcAddress(const std::string& name, const Range* range) :
		IpAddress(name, SrcAddress::dt(range), range)
	{
	}


	DomainType SrcAddress::dt(const Range* range)
	{
		switch (range->nbits()) {
		case 32: return DomainType::SrcAddress4;
		case 128: return DomainType::SrcAddress6;
		default:
			throw std::invalid_argument("internal error : nbits must be 32 or 128");
		}
	}


	SrcAddress::SrcAddress(const std::string& name, uint32_t lbound, uint32_t ubound, IPAddressFormat format) :
		SrcAddress(name, SrcAddress4Domain::create_range(lbound, ubound))
	{
	}


	SrcAddress::SrcAddress(const std::string& name, uint128_t lbound, uint128_t ubound, IPAddressFormat format) :
		SrcAddress(name, SrcAddress6Domain::create_range(lbound, ubound, format == IPAddressFormat::IP4Format))
	{
	}


	SrcAddress::SrcAddress(const SrcAddress& address) :
		SrcAddress(address.name(), address)
	{
	}


	SrcAddress::SrcAddress(const std::string& name, const IpAddress& address) :
		SrcAddress(name, address.value().range().clone())
	{
	}


	const SrcAddress* SrcAddress::create(const std::string& name, const std::string& addr, IPAddressModel ip_model, bool strict)
	{
		switch (ip_model) {
		case IPAddressModel::IP4Model:
		{
			// Only IPv4 syntax is accepted.
			auto range_4{ decode_ipv4_network_address(addr, strict) };
			return new SrcAddress(name, std::get<1>(range_4), std::get<2>(range_4), IPAddressFormat::IP4Format);
		}

		case IPAddressModel::IP6Model:
		{
			if (strict) {
				// Only IPv6 syntax is accepted.
				auto range_6{ decode_ipv6_network_address(addr, strict) };
				return new SrcAddress(name, std::get<1>(range_6), std::get<2>(range_6), IPAddressFormat::IP6Format);
			}
			else {
				// IPv6 and IPv4 syntax are accepted.
				try {
					auto range{ decode_ipv6_network_address(addr, strict) };
					return new SrcAddress(name, std::get<1>(range), std::get<2>(range), IPAddressFormat::IP6Format);
				}
				catch (IPAddressError e) {
				}

				auto range{ decode_ipv4_network_address(addr, strict) };
				return new SrcAddress(name, uint128_t(std::get<1>(range)), uint128_t(std::get<2>(range)), IPAddressFormat::IP4Format);
			}
		}

		default:
		{
			// IPv6 and IPv4 syntax are accepted.
			try {
				auto range{ decode_ipv6_network_address(addr, strict) };
				return new SrcAddress(name, std::get<1>(range), std::get<2>(range), IPAddressFormat::IP6Format);
			}
			catch (IPAddressError e) {
			}

			auto range{ decode_ipv4_network_address(addr, strict) };
			return new SrcAddress(name, std::get<1>(range), std::get<2>(range), IPAddressFormat::IP4Format);
		}
		}
	}


	const SrcAddress* SrcAddress::any4(IPAddressModel ip_model)
	{
		return new SrcAddress(
			ip_model == IPAddressModel::IP64Model ? "any4" : "any",
			SrcAddress4Domain::create_full_range()
		);
	}


	const SrcAddress* SrcAddress::any6(IPAddressModel ip_model)
	{
		return new SrcAddress(
			ip_model == IPAddressModel::IP64Model ? "any6" : "any",
			SrcAddress6Domain::create_full_range()
		);
	}


	SrcAnyAddressGroup::~SrcAnyAddressGroup()
	{
		// delete the "any" address allocated in the child constructor
		parse([](const SrcAddress* item){ delete item; });
	}


	bdd SrcAnyAddressGroup::make_bdd() const
	{
		return bddtrue;
	}


	SrcAny4AddressGroup::SrcAny4AddressGroup():
		SrcAnyAddressGroup("$src-any4-group", SrcAddress::any4(IPAddressModel::IP4Model))
	{
	}


	SrcAddressGroup* SrcAny4AddressGroup::clone() const
	{
		return new SrcAny4AddressGroup();
	}


	SrcAny6AddressGroup::SrcAny6AddressGroup() :
		SrcAnyAddressGroup("$src-any6-group", SrcAddress::any6(IPAddressModel::IP6Model))
	{
	}

	SrcAddressGroup* SrcAny6AddressGroup::clone() const
	{
		return new SrcAny6AddressGroup();
	}


	SrcAny64AddressGroup::SrcAny64AddressGroup() :
		SrcAnyAddressGroup("$src-any64-group")
	{
		add_member(SrcAddress::any4(IPAddressModel::IP64Model));
		add_member(SrcAddress::any6(IPAddressModel::IP64Model));
	}


	SrcAddressGroup* SrcAny64AddressGroup::clone() const
	{
		return new SrcAny64AddressGroup();
	}


	DstAddress::DstAddress(const std::string& name, const Range* range) :
		IpAddress(name, DstAddress::dt(range), range)
	{
	}


	DomainType DstAddress::dt(const Range* range)
	{
		switch (range->nbits()) {
		case 32: return DomainType::DstAddress4;
		case 128: return DomainType::DstAddress6;
		default:
			throw std::invalid_argument("internal error : nbits must be 32 or 128");
		}
	}


	DstAddress::DstAddress(const std::string& name, uint32_t lbound, uint32_t ubound, IPAddressFormat format) :
		DstAddress(name, DstAddress4Domain::create_range(lbound, ubound))
	{
	}


	DstAddress::DstAddress(const std::string & name, uint128_t lbound, uint128_t ubound, IPAddressFormat format) :
		DstAddress(name, DstAddress6Domain::create_range(lbound, ubound, format == IPAddressFormat::IP4Format))
	{
	}


	DstAddress::DstAddress(const DstAddress& address) :
		DstAddress(address.name(), address)
	{
	}


	DstAddress::DstAddress(const std::string& name, const IpAddress& address) :
		DstAddress(name, address.value().range().clone())
	{
	}


	const DstAddress* DstAddress::create(const std::string& name, const std::string& addr, IPAddressModel ip_model, bool strict)
	{
		switch (ip_model) {
		case IPAddressModel::IP4Model:
		{
			// Only IPv4 syntax is accepted.
			auto range_4{ decode_ipv4_network_address(addr, strict) };
			return new DstAddress(name, std::get<1>(range_4), std::get<2>(range_4), IPAddressFormat::IP4Format);
		}

		case IPAddressModel::IP6Model:
		{
			if (strict) {
				// Only IPv6 syntax is accepted.
				auto range_6{ decode_ipv6_network_address(addr, strict) };
				return new DstAddress(name, std::get<1>(range_6), std::get<2>(range_6), IPAddressFormat::IP6Format);
			}
			else {
				// IPv6 and IPv4 syntax are accepted.
				try {
					auto range{ decode_ipv6_network_address(addr, strict) };
					return new DstAddress(name, std::get<1>(range), std::get<2>(range), IPAddressFormat::IP6Format);
				}
				catch (IPAddressError e) {
				}

				auto range{ decode_ipv4_network_address(addr, strict) };
				return new DstAddress(name, uint128_t(std::get<1>(range)), uint128_t(std::get<2>(range)), IPAddressFormat::IP4Format);
			}
		}

		default:
		{
			// IPv6 and IPv4 syntax are accepted.
			try {
				auto range{ decode_ipv6_network_address(addr, strict) };
				return new DstAddress(name, std::get<1>(range), std::get<2>(range), IPAddressFormat::IP6Format);
			}
			catch (IPAddressError e) {
			}

			auto range{ decode_ipv4_network_address(addr, strict) };
			return new DstAddress(name, std::get<1>(range), std::get<2>(range), IPAddressFormat::IP4Format);
		}
		}
	}


	const DstAddress* DstAddress::any4(IPAddressModel ip_model)
	{
		return new DstAddress(
			ip_model == IPAddressModel::IP64Model ? "any4" : "any",
			DstAddress4Domain::create_full_range()
		);
	}


	const DstAddress* DstAddress::any6(IPAddressModel ip_model)
	{
		return new DstAddress(
			ip_model == IPAddressModel::IP64Model ? "any6" : "any",
			DstAddress6Domain::create_full_range()
		);
	}


	DstAnyAddressGroup::~DstAnyAddressGroup()
	{
		// delete the "any" address allocated in the child constructor
		parse([](const DstAddress* item){ delete item; });
	}


	bdd DstAnyAddressGroup::make_bdd() const
	{
		return bddtrue;
	}


	DstAny4AddressGroup::DstAny4AddressGroup():
		DstAnyAddressGroup("$dst-any4-group", DstAddress::any4(IPAddressModel::IP4Model))
	{
	}


	DstAddressGroup* DstAny4AddressGroup::clone() const
	{
		return new DstAny4AddressGroup();
	}


	DstAny6AddressGroup::DstAny6AddressGroup() :
		DstAnyAddressGroup("$dst-any6-group", DstAddress::any6(IPAddressModel::IP6Model))
	{
	}


	DstAddressGroup* DstAny6AddressGroup::clone() const
	{
		return new DstAny6AddressGroup();
	}


	DstAny64AddressGroup::DstAny64AddressGroup() :
		DstAnyAddressGroup("$dst-any64-group")
	{
		add_member(DstAddress::any4(IPAddressModel::IP64Model));
		add_member(DstAddress::any6(IPAddressModel::IP64Model));
	}


	DstAddressGroup* DstAny64AddressGroup::clone() const
	{
		return new DstAny64AddressGroup();
	}

}
