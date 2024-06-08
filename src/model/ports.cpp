/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#include "ports.h"
#include "domains.h"

namespace fwm {

	class AnyPorts final : public Ports
	{
	public:
		/* Allocate a node that represents any tcp/udp port or any icmp type.  The Domain type
		 * is not important here as we overload make_bdd to returns a bddtrue value.
		 */
		AnyPorts() :
			Ports(DomainType::TcpDstPort, 0, Domains::get(DomainType::TcpDstPort).upper())
		{
		}

		virtual bdd make_bdd() const override
		{
			return bddtrue;
		}
	};


	Ports::Ports(const DomainType dt, uint32_t lower, uint32_t upper) :
		Interval(dt, lower, upper)
	{
	}


	const Ports* Ports::any()
	{
		return new AnyPorts();
	}


	TcpPorts::TcpPorts(uint32_t lower, uint32_t upper) :
		Ports(DomainType::TcpDstPort, lower, upper)
	{
	}


	UdpPorts::UdpPorts(uint32_t lower, uint32_t upper) :
		Ports(DomainType::UdpDstPort, lower, upper)
	{
	}


	IcmpPorts::IcmpPorts(uint32_t lower, uint32_t upper) :
		Ports(DomainType::IcmpType, lower, upper)
	{
	}

}
