/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#include "model/serviceparser.h"

#include <utility>

#include "model/domains.h"
#include "tools/strutil.h"
#include "fmt/core.h"


namespace fwm {

	ServiceError::ServiceError(const std::string& proto_port) :
		std::runtime_error(
			fmt::format("'{}' is not a valid protocol/port specification", proto_port)
		),
		_proto_port{ proto_port }
	{
	}

	// forward declaration
	static bool parse_port_range(const std::string& range, uint16_t& lvalue, uint16_t& uvalue, uint16_t max_value);


	std::tuple<ProtocolType, uint16_t, uint16_t> parse_protocol_port(const std::string& proto_port)
	{
		std::vector<std::string> parts;
		ProtocolType pt;
		uint16_t port_lower;
		uint16_t port_upper;

		if (proto_port.empty())
			throw ServiceError(proto_port);

		if (rat::split(proto_port, '/', parts) > 2)
			throw ServiceError(proto_port);

		if (rat::iequal(parts[0], TcpProtocol::name())) {
			pt = ProtocolType::TCP;
			port_lower = 0;
			port_upper = DstTcpPortDomain::max();
		}
		else if (rat::iequal(parts[0], UdpProtocol::name())) {
			pt = ProtocolType::UDP;
			port_lower = 0;
			port_upper = DstUdpPortDomain::max();
		}
		else if (rat::iequal(parts[0], IcmpProtocol::name())) {
			pt = ProtocolType::ICMP;
			port_lower = 0;
			port_upper = IcmpTypeDomain::max();
		}
		else
			throw ServiceError(proto_port);

		if (parts.size() == 1) {
			// noop, keep lower and upper port
		}
		else if (!parse_port_range(parts[1], port_lower, port_upper, port_upper))
			throw ServiceError(proto_port);

		return std::make_tuple(pt, port_lower, port_upper);
	}


	static bool parse_port_range(const std::string& range, uint16_t& lvalue, uint16_t& uvalue, uint16_t max_value)
	{
		std::vector<std::string> tmp;
		if (rat::split(range, '-', tmp) > 2)
			return false;

		long value;
		if (!rat::str2num(tmp[0], 10, 0, max_value, value))
			return false;
		lvalue = static_cast<uint16_t>(value);

		if (tmp.size() == 2) {
			if (!rat::str2num(tmp[1], 10, 0, max_value, value))
				return false;
			uvalue = static_cast<uint16_t>(value);
		}
		else {
			uvalue = lvalue;
		}

		return lvalue <= uvalue;
	}


	bool is_protocol_port(const std::string& proto_port)
	{
		bool valid{ true };

		try {
			parse_protocol_port(proto_port);
		}
		catch (const ServiceError&) {
			valid = false;
		}

		return valid;
	}

}
