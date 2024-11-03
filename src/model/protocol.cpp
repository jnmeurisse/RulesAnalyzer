/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#include "model/protocol.h"
#include "model/domains.h"

namespace fwm {

	class AnyProtocol final : public Protocol
	{
	public:
		AnyProtocol() :
			Protocol(ProtocolType::ANY, ProtocolDomain::create_full_range())
		{
		}


		virtual const Protocol* clone() const override
		{
			return new AnyProtocol();
		}


		virtual std::string to_string() const override {
			return "any";
		}


		virtual bdd make_bdd() const override
		{
			return bddtrue;
		}
	};


	Protocol::Protocol(ProtocolType protocol_type, const Range* range) :
		_protocol_type{ protocol_type },
		Mvalue(DomainType::Protocol, range)
	{
	}


	const Protocol* Protocol::any()
	{
		return new AnyProtocol();
	}


	std::string Protocol::to_string() const
	{
		return Mvalue::to_string();
	}


	TcpProtocol::TcpProtocol(const Range* range) :
		Protocol(ProtocolType::TCP, range)
	{
	}


	TcpProtocol::TcpProtocol() :
		TcpProtocol(ProtocolDomain::create_singleton(static_cast<int>(ProtocolType::TCP)))
	{
	}


	const Protocol* TcpProtocol::clone() const
	{
		return new TcpProtocol(range().clone());
	}


	std::string TcpProtocol::to_string() const
	{
		return name();
	}


	UdpProtocol::UdpProtocol(const Range* range) :
		Protocol(ProtocolType::UDP, range)
	{
	}


	UdpProtocol::UdpProtocol() :
		UdpProtocol(ProtocolDomain::create_singleton(static_cast<int>(ProtocolType::UDP)))
	{
	}


	const Protocol* UdpProtocol::clone() const
	{
		return new UdpProtocol(range().clone());
	}


	std::string UdpProtocol::to_string() const
	{
		return name();
	}


	IcmpProtocol::IcmpProtocol(const Range* range) :
		Protocol(ProtocolType::ICMP, range)
	{
	}


	IcmpProtocol::IcmpProtocol() :
		IcmpProtocol(ProtocolDomain::create_singleton(static_cast<int>(ProtocolType::ICMP)))
	{
	}


	const Protocol* IcmpProtocol::clone() const
	{
		return new IcmpProtocol(range().clone());
	}


	std::string IcmpProtocol::to_string() const
	{
		return name();
	}

}
