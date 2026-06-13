/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#pragma once
#include "global.h"
#include "model/mnode.h"
#include "model/mvalue.h"


#include <stdexcept>
#include <string>


namespace fwm {

	enum class IPAddressModel {
		IP4Model,
		IP6Model,
		IP64Model
	};


	std::string to_string(IPAddressModel address_model);


	enum class IPAddressType {
		Address,
		Subnet,
		Range
	};


	enum class IPAddressFormat {
		IP4Format,
		IP6Format
	};


	/* Returns the address type of an IP address. */
	IPAddressType get_ip_address_type(const std::string& addr, IPAddressModel ip_model, bool strict);


	class IPAddressError : public std::runtime_error
	{
	public:
		IPAddressError(const std::string& address, const std::string& reason);

		inline const std::string& address() const noexcept { return _address; }
		inline const std::string& reason() const noexcept { return _reason; }

	private:
		const std::string _address;
		const std::string _reason;
	};


	/**
	 * IpAddress represents a range of IP addresses.
	*
	*/
	class IpAddress abstract : public NamedMnode
	{
	public:
		/**
		 * Creates a binary decision diagram for this address.
		*/
		virtual bdd make_bdd() const override;

		/**
		 * Returns the address value.
		*/
		const Mvalue& value() const;

		/**
		 * Returns the address as a string.
		*/
		virtual std::string to_string() const override;

		/**
		 * Returns the address type (single address, subnet or range)
		*/
		IPAddressType at() const;

		/**
		 * Returns the IP address version (4 or 6).
		*/
		int version() const;

		/**
		 * Returns true if the string is a valid IP address or IP address range.
		*/
		static bool is_valid_ip(const std::string& addr, IPAddressModel ip_model, bool strict);


	protected:
		/*
		 * Allocates a new IP address.
		 *
		 * @param name The name of the IP address
		 * @param dt The domain type of this IP address (source, destination, IPv4 or IPv6)
		 * @param range The range of IP addresses
		*/
		IpAddress(const std::string& name, DomainType dt, const Range* range);

	private:
		// The address definition.
		const MvaluePtr _address_value;
	};


	/**
	 * An AddressList represents a list of IP source and destination addresses.
	*/
	using AddressList = NamedMnodeList<IpAddress>;
	using AddressListPtr = std::shared_ptr<AddressList>;

}
