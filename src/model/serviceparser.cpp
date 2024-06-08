/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#include <utility>

#include "domains.h"
#include "serviceparser.h"
#include "tools/strutil.h"

namespace fwm {

	ServiceError::ServiceError(const std::string& proto_port) :
		std::runtime_error(
			string_format("'%s' is not a valid protocol/port specification", proto_port.c_str())
		),
		_proto_port{ proto_port }
	{
	}

	// forward declaration
	static bool parse_port_range(const std::string& range, uint16_t& lvalue, uint16_t& uvalue, DomainType dt);


	std::tuple<ProtocolType, uint16_t, uint16_t> parse_protocol_port(const std::string& proto_port)
	{
		std::vector<std::string> parts;
		ProtocolType pt;
		DomainType dt;
		uint16_t port_lower;
		uint16_t port_upper;

		if (proto_port.empty())
			throw ServiceError(proto_port);

		if (split(proto_port, '/', parts) > 2)
			throw ServiceError(proto_port);

		if (iequal(parts[0], TcpProtocol::NAME)) {
			pt = ProtocolType::TCP;
			dt = DomainType::TcpDstPort;
		}
		else if (iequal(parts[0], UdpProtocol::NAME)) {
			pt = ProtocolType::UDP;
			dt = DomainType::UdpDstPort;
		}
		else if (iequal(parts[0], IcmpProtocol::NAME)) {
			pt = ProtocolType::ICMP;
			dt = DomainType::IcmpType;
		}
		else
			throw ServiceError(proto_port);

		if (parts.size() == 1) {
			port_lower = 0;
			port_upper = Domains::get(dt).upper();
		}
		else if (parts[1] == "any") {
			port_lower = 0;
			port_upper = Domains::get(dt).upper();
		}
		else if (parts[1] == "dynamic") {
			port_lower = 0;
			port_upper = 0;
		}
		else if (!parse_port_range(parts[1], port_lower, port_upper, dt))
			throw ServiceError(proto_port);

		return std::make_tuple(pt, port_lower, port_upper);
	}


	static bool parse_port_range(const std::string& range, uint16_t& lvalue, uint16_t& uvalue, DomainType dt)
	{
		const Domain& domain{ Domains::get(dt) };

		std::vector<std::string> tmp;
		if (split(range, '-', tmp) > 2)
			return false;

		long value;
		if (!str2num(tmp[0], 10, 0, domain.upper(), value))
			return false;
		lvalue = static_cast<uint16_t>(value);

		if (tmp.size() == 2) {
			if (!str2num(tmp[1], 10, 0, domain.upper(), value))
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

