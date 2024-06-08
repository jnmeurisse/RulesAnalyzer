/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#include "protocol.h"
#include "domains.h"

namespace fwm {

	class AnyProtocol final : public Protocol
	{
	public:
		AnyProtocol() :
			Protocol(DomainType::Protocol, 0, Domains::get(DomainType::Protocol).upper())
		{
		}

		virtual std::string to_string() const override {
			return "any";
		}

		virtual bdd make_bdd() const override
		{
			return bddtrue;
		}
	};


	Protocol::Protocol(const DomainType dt, uint32_t lower, uint32_t upper) :
		Interval(dt, lower, upper)
	{
	}


	Protocol::Protocol(const DomainType dt, ProtocolType protocol) :
		Protocol(dt, static_cast<int>(protocol), static_cast<int>(protocol))
	{
	}


	const Protocol* Protocol::any()
	{
		return new AnyProtocol();
	}


	std::string Protocol::to_string() const
	{
		return Interval::to_string();
	}


	TcpProtocol::TcpProtocol() :
		Protocol(DomainType::Protocol, ProtocolType::TCP)
	{
	}


	std::string TcpProtocol::to_string() const
	{
		return TcpProtocol::NAME;
	}


	UdpProtocol::UdpProtocol() :
		Protocol(DomainType::Protocol, ProtocolType::UDP)
	{
	}


	std::string UdpProtocol::to_string() const
	{
		return UdpProtocol::NAME;
	}


	IcmpProtocol::IcmpProtocol() :
		Protocol(DomainType::Protocol, ProtocolType::ICMP)
	{
	}


	std::string IcmpProtocol::to_string() const
	{
		return IcmpProtocol::NAME;
	}


	const std::string TcpProtocol::NAME = "tcp";
	const std::string UdpProtocol::NAME = "udp";
	const std::string IcmpProtocol::NAME = "icmp";

}