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
#include <memory>
#include <string>
#include "model/ipaddress.h"
#include "model/group.h"
#include "model/range.h"
#include "model/table.h"


namespace fwm {

	/**
	 * SrcAddress represents an IP address range used as source address.
	*/
	class SrcAddress final : public IpAddress
	{
	public:
		using Ptr = std::shared_ptr<const SrcAddress>;

		/**
		 * Copy constructor.
		 *
		 * @param address The source address to copy
		*/
		SrcAddress(const SrcAddress& address);

		/**
		 * Allocates a source address from a source or destination address.
		 *
		 * @param name The name of address.
		 * @param address The IP address to copy.  This address can be a source or destination
		 *        address.
		*/
		SrcAddress(const std::string& name, const IpAddress& address);
	
		/**
		 * Allocates a new source address
		 *
		 * @param name The name of the address.
		 * @param addr The address definition.
		 * @param ip_model The model of IP address allocated (IPv4 or IPv6)
		 * @param strict Do we raise exception if the address and mask do not match.
		 *
		 * @return A source address
		 * @throws IPAddressError
		 */
		static Ptr create(const std::string& name, const std::string& addr, IPAddressModel ip_model, bool strict);

		/**
		 * Allocates a source address that represents all IPv4 addresses.
		*/
		static Ptr any4(IPAddressModel ip_model);

		/**
		 * Allocates a source address that represents all IPv6 addresses.
		*/
		static Ptr any6(IPAddressModel ip_model);

	private:
		/**
		 * Allocates an IPv4 source address range.
		*/
		SrcAddress(const std::string& name, uint32_t lbound, uint32_t ubound, IPAddressFormat format);

		/**
		 * Allocates an IPv6 source address range.
		*/
		SrcAddress(const std::string& name, uint128_t lbound, uint128_t ubound, IPAddressFormat format);

		/**
		 * Allocates an IP source address from a given range.
		*/
		SrcAddress(const std::string& name, const Range* range);

		/**
		 * Returns the domain type of a given range.
		*/
		static DomainType dt(const Range* range);
	};

	using SrcAddressPtr = SrcAddress::Ptr;


	/**
	 * SrcAddressGroup represents a hierarchy of groups of source IP addresses.
	*/
	using SrcAddressGroup = Group<SrcAddress>;
	using SrcAddressGroupPtr = std::shared_ptr<const SrcAddressGroup>;


	class SrcAnyAddressGroup abstract : public SrcAddressGroup
	{
	public:
		using SrcAddressGroup::SrcAddressGroup;

		virtual bdd make_bdd() const override;
	};


	/**
	 * SrcAny4AddressGroup represents a hierarchy of groups of source IPv4 addresses containing
	 * only an any IPv4 address.
	*/
	class SrcAny4AddressGroup final : public SrcAnyAddressGroup
	{
	public:
		SrcAny4AddressGroup();
	};


	/**
	 * SrcAny6AddressGroup represents a hierarchy of groups of source IPv6 addresses containing
	 * only an any IPv6 address.
	*/
	class SrcAny6AddressGroup final : public SrcAnyAddressGroup
	{
	public:
		SrcAny6AddressGroup();
	};


	/**
	 * SrcAny64AddressGroup represents a hierarchy of groups of source addresses containing
	 * only one any IPv4 address and one any IPv6 address.
	*/
	class SrcAny64AddressGroup final : public SrcAnyAddressGroup
	{
	public:
		SrcAny64AddressGroup();
	};


	/**
	 * DstAddress represents an IP address range used as destination address.
	*/
	class DstAddress final : public IpAddress
	{
	public:
		using Ptr = std::shared_ptr<const DstAddress>;

		/**
		 * Copy constructor.
		 *
		 * @param address The destination address to copy
		*/
		DstAddress(const DstAddress& address);

		/**
		 * Allocates a destination address from a source or destination address.
		 *
		 * @param name The name of address.
		 * @param address The IP address to copy.  This address can be a source or destination
		 *        address.
		*/
		DstAddress(const std::string& name, const IpAddress& address);

		/**
		 * Allocates a new destination address
		 *
		 * @param name The name of the address.
		 * @param addr The address definition.
		 * @param ip_model The model of IP address allocated (IPv4 or IPv6)
		 * @param strict Do we raise exception if the address and mask do not match.
		 *
		 * @return A destination address
		 * @throws IPAddressError
		 */
		static Ptr create(const std::string& name, const std::string& addr, IPAddressModel ip_model, bool strict);

		/**
		 * Allocates a destination address that represents all IPv4 addresses.
		*/
		static Ptr any4(IPAddressModel ip_model);

		/**
		 * Allocates a destination address that represents all IPv6 addresses.
		*/
		static Ptr any6(IPAddressModel ip_model);

	private:
		/**
		 * Allocates an IPv4 destination address range.
		*/
		DstAddress(const std::string& name, uint32_t lbound, uint32_t ubound, IPAddressFormat format);

		/**
		 * Allocates an IPv6 destination address range.
		*/
		DstAddress(const std::string& name, uint128_t lbound, uint128_t ubound, IPAddressFormat format);

		/**
		 * Allocates an IP destination address from a given range.
		*/
		DstAddress(const std::string& name, const Range* range);

		/**
		 * Returns the domain type of a given range.
		*/
		static DomainType dt(const Range* range);
	};

	using DstAddressPtr = DstAddress::Ptr;


	/**
	 * DstAddressGroup represents a hierarchy of groups of destination IP addresses.
	*/
	using DstAddressGroup = Group<DstAddress>;
	using DstAddressGroupPtr = std::shared_ptr<const DstAddressGroup>;


	class DstAnyAddressGroup abstract : public DstAddressGroup
	{
	public:
		using DstAddressGroup::DstAddressGroup;

		virtual bdd make_bdd() const override;
	};


	/**
	 * DstAny4AddressGroup represents a hierarchy of groups of destination IPv4 addresses containing
	 * only an any IPv4 address.
	*/
	class DstAny4AddressGroup final : public DstAnyAddressGroup
	{
	public:
		DstAny4AddressGroup();
	};


	/**
	 * DstAny6AddressGroup represents a hierarchy of groups of destination IPv4 addresses containing
	 * only an any IPv6 address.
	*/
	class DstAny6AddressGroup final : public DstAnyAddressGroup
	{
	public:
		DstAny6AddressGroup();
	};


	/**
	 * DstAny64AddressGroup represents a hierarchy of groups of destination addresses containing
	 * only one any IPv4 address and one any IPv6 address.
	*/
	class DstAny64AddressGroup final : public DstAnyAddressGroup
	{
	public:
		DstAny64AddressGroup();
	};

}
