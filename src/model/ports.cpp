/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#include "model/ports.h"


namespace fwm {

	class AnyPorts final : public Ports
	{
	public:
		/* Allocate a node that represents any tcp/udp port or any icmp type.  The Domain type
		 * is not important here as we overload make_bdd to returns a bddtrue value.
		 */
		AnyPorts() :
			Ports(DomainType::DstTcpPort, PortDomain::create_full_range())
		{
		}

		virtual Ports* clone() const override
		{
			return new AnyPorts();
		}

		virtual bdd make_bdd() const override
		{
			return bddtrue;
		}
	};


	Ports::Ports(const DomainType dt, const Range* range) :
		Mvalue(dt, range)
	{
	}


	const Ports* Ports::any()
	{
		return new AnyPorts();
	}


	TcpPorts::TcpPorts(const Range* range) :
		Ports(DomainType::DstTcpPort, range)
	{
	}


	TcpPorts::TcpPorts(uint16_t lbound, uint16_t ubound) :
		TcpPorts(DstTcpPortDomain::create_range(lbound, ubound))
	{
	}


	const Ports* TcpPorts::clone() const
	{
		return new TcpPorts(range().clone());
	}


	UdpPorts::UdpPorts(const Range* range) :
		Ports(DomainType::DstUdpPort, range)
	{
	}


	UdpPorts::UdpPorts(uint16_t lbound, uint16_t ubound) :
		UdpPorts(DstTcpPortDomain::create_range(lbound, ubound))
	{
	}


	const Ports* UdpPorts::clone() const
	{
		return new UdpPorts(range().clone());
	}


	IcmpPorts::IcmpPorts(const Range* range) :
		Ports(DomainType::IcmpType, range)
	{
	}


	IcmpPorts::IcmpPorts(uint16_t lbound, uint16_t ubound) :
		IcmpPorts(IcmpTypeDomain::create_range(lbound, ubound))
	{
	}


	const Ports* IcmpPorts::clone() const
	{
		return new IcmpPorts(range().clone());
	}

}
