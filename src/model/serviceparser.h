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
#include <stdexcept>
#include <string>
#include <tuple>

#include "model/protocol.h"


namespace fwm {

	class ServiceError : public std::runtime_error
	{
	public:
		ServiceError(const std::string& proto_port);

		inline const std::string& proto_port() const noexcept { return _proto_port; }

	private:
		std::string _proto_port;
	};


	/*
	 * Parses a protocol/port service.
	 *
	 * The proto_port syntax is protocol{/ports}.
	 * Examples
	 *    TCP
	 *    UDP/22
	 *    UDP/23-33
	 *    ICMP/0
	*/
	std::tuple<ProtocolType, uint16_t, uint16_t> parse_protocol_port(const std::string& proto_port);


	/* Returns true if the string is a valid protocol/port service
	*/
	bool is_protocol_port(const std::string& proto_port);

}