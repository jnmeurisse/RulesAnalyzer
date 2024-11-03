/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#pragma once
#include "global.h"

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

}
