/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#pragma once
#include <memory>
#include "interval.h"
#include "domain.h"

namespace fwm {


	enum class ProtocolType {
		UDP = 0,
		TCP = 1,
		ICMP = 2
	};


	class Protocol : public Interval {
	public:
		static const Protocol* any();
		virtual std::string to_string() const override;

	protected:
		Protocol(const DomainType dt, uint32_t lower, uint32_t upper);
		Protocol(const DomainType dt, ProtocolType protocol);
	};


	class TcpProtocol final : public Protocol {
	public:
		TcpProtocol();
		virtual std::string to_string() const override;

		static const std::string NAME;
	};


	class UdpProtocol final : public Protocol {
	public:
		UdpProtocol();
		virtual std::string to_string() const override;

		static const std::string NAME;
	};


	class IcmpProtocol final : public Protocol {
	public:
		IcmpProtocol();
		virtual std::string to_string() const override;

		static const std::string NAME;
	};

}
