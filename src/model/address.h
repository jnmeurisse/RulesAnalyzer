/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#pragma once
#include <memory>
#include <string>
#include <tuple>

#include "mnode.h"
#include "interval.h"
#include "ipv4parser.h"
#include "group.h"
#include "table.h"


namespace fwm {

	class Ipv4Address : public NamedMnode
	{
	public:
		/* Creates a binary decision diagram for this address.
		*/
		virtual bdd make_bdd() const override;

		/* returns the address type.
		*/
		inline Ipv4AddressType at() const noexcept { return _at; }

		/* returns the address range.
		*/
		const Interval& interval() const;

		/* returns the address as a string.
		*/
		virtual std::string to_string() const override;

	protected:
		Ipv4Address(const std::string& name, DomainType dt, Ipv4AddressType at, uint32_t lower, uint32_t upper);

	private:
		/* the address type (a single address, a subnet or a range) */
		const Ipv4AddressType _at;

		/* the address definition */
		const Interval _address;
	};


	/* AddressGroup represents a collection of Ipv4 source and destination addresses.
	*  Warning : do not compare the bdd of this group with other groups of the same class
	*  or with another group of addresses.  The comparison will give unexpected results
	*  since source and destination domains are not the same.  To avoid any programming
	*  errors, the make_bdd raises an exception.
	*/
	class AddressGroup : public Group<Ipv4Address>
	{
	public:
		AddressGroup() = delete;
		explicit AddressGroup(const std::string& name);

		virtual bdd make_bdd() const override;
	};

	using AddressGroupPtr = std::unique_ptr<AddressGroup>;


	/* SrcAddress represents a source IPv4 address.
	*/
	class SrcAddress final : public Ipv4Address
	{
	public:
		SrcAddress(const std::string& name, Ipv4AddressType at, uint32_t lower, uint32_t upper);

		static SrcAddress* create(const std::string& name, const std::string& addr, bool strict);
		static SrcAddress* any();
	};


	/* SrcAddressGroup represents a group of source IPv4 address.
	*/
	class SrcAddressGroup : public Group<SrcAddress>
	{
	public:
		SrcAddressGroup() = delete;

		/* Allocates an empty group.
		*/
		SrcAddressGroup(const std::string& name);

		/* Allocates and initialize a group with a unique address.
		*/
		SrcAddressGroup(const std::string& name, const SrcAddress* address);
	};

	using SrcAddressGroupPtr = std::unique_ptr<SrcAddressGroup>;


	/* DstAddress represents a destination IPv4 address.
	*/
	class DstAddress final : public Ipv4Address
	{
	public:
		DstAddress(const std::string& name, Ipv4AddressType at, uint32_t lower, uint32_t upper);

		static DstAddress* create(const std::string& name, const std::string& addr, bool strict);
		static DstAddress* any();
	};


	/* DstAddressGroup represents a group of source IPv4 address.
	*/
	class DstAddressGroup : public Group<DstAddress>
	{
	public:
		DstAddressGroup() = delete;

		/* Allocates an empty group.
		*/
		DstAddressGroup(const std::string& name);

		/* Allocates and initialize a group with a unique address.
		*/
		DstAddressGroup(const std::string& name, const DstAddress* address);
	};

	using DstAddressGroupPtr = std::unique_ptr<DstAddressGroup>;

}
