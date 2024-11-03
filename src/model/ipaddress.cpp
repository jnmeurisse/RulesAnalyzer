/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#include "model/ipaddress.h"
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


	IPAddressError::IPAddressError(const std::string& address, const std::string& reason) :
			std::runtime_error(
				fmt::format("'{}' is not a valid IP address : '{}'", address, reason)
			),
			_address{ address },
			_reason{ reason }
		{
		}

}
