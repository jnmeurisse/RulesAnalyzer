/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#pragma once
#include "global.h"

#include "model/ipaddress.h"


namespace fwm {

	class ModelConfig
	{
	public:
		ModelConfig() :
			ip_model{ IPAddressModel::IP4Model },
			strict_ip_parser{ true }
		{}

		// address model (IPv4 or IPv6 or combined IPv4 and v6 addresses).
		IPAddressModel ip_model;

		// strict IP address parser.
		bool strict_ip_parser;
	};

}
