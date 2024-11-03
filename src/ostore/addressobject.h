/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#pragma once
#include "global.h"

#include <string>
#include <vector>

#include "ostore/firewallobject.h"


namespace fos {

	enum class AddressType {
		ipmask,
		iprange,
		fqdn
	};


	/* An address object
	*/
	class AddressObject abstract : public FirewallObject
	{
	public:
		/* Returns a string representation of this object.
		*/
		virtual std::string to_string() const override;

		/* Returns the address definitions.
		*/
		inline const std::vector<std::string>& addresses() const noexcept { return _addresses; }

		/* Returns true if it is a multi addresses.
		*/
		inline bool is_multi() const noexcept { return _addresses.size() > 1; }

		/* Returns the address type
		*/
		inline AddressType type() const noexcept { return _type; };

	protected:
		AddressObject(const std::string& name, const AddressType type, const std::vector<std::string>& addresses);

	private:
		const AddressType _type;
		const std::vector<std::string> _addresses;
	};


	class IpmaskAddressObject final : public AddressObject
	{
	public:
		IpmaskAddressObject(const std::string& name, const std::vector<std::string>& addresses);
	};


	class IprangeAddressObject final : public AddressObject
	{
	public:
		IprangeAddressObject(const std::string& name, const std::vector<std::string>& addresses);
	};


	class FqdnAddressObject final : public AddressObject
	{
	public:
		FqdnAddressObject(const std::string& name, const std::string& address);
	};
}
