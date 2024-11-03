/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#pragma once
#include "global.h"

#include <string>

#include "model/domain.h"
#include "model/mvalue.h"
#include "model/range.h"


namespace fwm {

	enum class ProtocolType {
		ANY = -1,
		UDP = 0,
		TCP = 1,
		ICMP = 2
	};


	class Protocol abstract : public Mvalue {
	public:
		static const Protocol* any();
		virtual const Protocol* clone() const = 0;

		inline ProtocolType pt() const noexcept { return _protocol_type; }

		virtual std::string to_string() const override;

	protected:
		Protocol(ProtocolType protocol_type, const Range* range);

	private:
		const ProtocolType _protocol_type;
	};


	class TcpProtocol final : public Protocol {
	public:
		TcpProtocol();
		virtual const Protocol* clone() const override;

		virtual std::string to_string() const override;

		static constexpr const char* name() { return "tcp"; }

	private:
		TcpProtocol(const Range* range);
	};


	class UdpProtocol final : public Protocol {
	public:
		UdpProtocol();
		virtual const Protocol* clone() const override;

		virtual std::string to_string() const override;

		static constexpr const char* name() { return "udp"; }

	private:
		UdpProtocol(const Range* range);
	};


	class IcmpProtocol final : public Protocol {
	public:
		IcmpProtocol();
		virtual const Protocol* clone() const override;

		virtual std::string to_string() const override;

		static constexpr const char* name() { return "icmp"; }

	private:
		IcmpProtocol(const Range* range);
	};

}
