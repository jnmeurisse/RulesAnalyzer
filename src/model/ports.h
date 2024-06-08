/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#pragma once
#include <memory>
#include "domain.h"
#include "interval.h"

namespace fwm {

	class Ports : public Interval
	{
	public:
		static const Ports* any();

	protected:
		Ports(const DomainType dt, uint32_t lower, uint32_t upper);
	};


	class TcpPorts final : public Ports
	{
	public:
		TcpPorts(uint32_t lower, uint32_t upper);
	};


	class UdpPorts final : public Ports
	{
	public:
		UdpPorts(uint32_t lower, uint32_t upper);
	};


	class IcmpPorts final : public Ports
	{
	public:
		IcmpPorts(uint32_t lower, uint32_t upper);
	};

}