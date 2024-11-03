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

#include "model/domain.h"
#include "model/mvalue.h"
#include "model/range.h"


namespace fwm {

	class Ports abstract : public Mvalue
	{
	public:
		virtual const Ports* clone() const = 0;
		static const Ports* any();

	protected:
		Ports(const DomainType dt, const Range* range);
	};


	class TcpPorts final : public Ports
	{
	public:
		TcpPorts(uint16_t lbound, uint16_t ubound);
		virtual const Ports* clone() const override;

	private:
		TcpPorts(const Range* range);
	};


	class UdpPorts final : public Ports
	{
	public:
		UdpPorts(uint16_t lbound, uint16_t ubound);
		virtual const Ports* clone() const override;

	private:
		UdpPorts(const Range* range);
	};


	class IcmpPorts final : public Ports
	{
	public:
		IcmpPorts(uint16_t lbound, uint16_t ubound);
		virtual const Ports* clone() const override;

	private:
		IcmpPorts(const Range* range);
	};

}
