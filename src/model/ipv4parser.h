/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#pragma once
#include <exception>
#include <string>
#include <tuple>


namespace fwm {

	enum class Ipv4AddressType {
		Address,
		Subnet,
		Range
	};


	class Ipv4AddressError : public std::runtime_error
	{
	public:
		Ipv4AddressError(const std::string& address, const std::string& reason);

		inline const std::string& address() const noexcept { return _address; }
		inline const std::string& reason() const noexcept { return _reason; }

	private:
		const std::string _address;
		const std::string _reason;
	};


	/*
	* Parse an ip address.
	*
	* The function accepts the following syntaxes :
	*  A sub network :
	*    '192.0.2.0/24'
	*    '192.0.2.0/255.255.255.0'
	*  A single address
	*    '192.0.2.1'
	*    '192.0.2.1/255.255.255.255'
	*    '192.0.2.1/32'
	* An address range
	*     '192.168.2.0-192.168.2.244'
	*
	* The method returns a tuple (type, start ip address, end ip address).
	*/
	std::tuple<Ipv4AddressType, uint32_t, uint32_t> parse_network_address(const std::string& addr, bool strict);


	/* Returns true if the string is a valid ip address or ip address range.
	*/
	bool is_network_address(const std::string& addr, bool strict);

}
