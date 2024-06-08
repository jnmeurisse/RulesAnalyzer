/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#pragma once
#include <exception>
#include <string>
#include <memory>

#include "mnode.h"
#include "group.h"
#include "interval.h"
#include "protocol.h"
#include "ports.h"
#include "table.h"

#include "tools/strutil.h"

namespace fwm {

	class Service : public NamedMnode
	{
	public:
		static Service* create(const std::string& name, const std::string& proto_port);
		static Service* any();

		/* Creates a binary decision diagram from this service.
		*/
		virtual bdd make_bdd() const override;

		/* Returns the protocol (tcp/upd/icmp).
		*/
		inline const Protocol& protocol() const noexcept { return *_protocol; }

		/* Returns the port(s).
		*/
		inline const Ports& ports() const noexcept { return *_ports; }

		/* Returns the service as a string.
		*/
		virtual std::string to_string() const override;

	protected:
		Service(const std::string& name, const Protocol* protocol, const Ports* ports);

	private:
		const std::unique_ptr<const Protocol> _protocol;
		const std::unique_ptr<const Ports> _ports;
	};


	class ServiceGroup : public Group<Service>
	{
	public:
		ServiceGroup() = delete;

		ServiceGroup(const std::string& name);
		ServiceGroup(const std::string& name, const Service* service);

		/* Returns true when a service group contains the default services of
		*  an application.
		*/
		virtual bool is_app_services() const;
	};

	using ServiceGroupPtr = std::unique_ptr<ServiceGroup>;


	class TcpService final : public Service {
	public:
		TcpService(const std::string& name, uint32_t lower_port, uint32_t upper_port);
	};


	class UdpService final : public Service {
	public:
		UdpService(const std::string& name, uint32_t lower_port, uint32_t upper_port);
	};


	class IcmpService : public Service {
	public:
		IcmpService(const std::string& name, uint32_t lower_port, uint32_t upper_port);
		IcmpService(const std::string& name, uint32_t type);
	};


}
