/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#include "service.h"

#include <algorithm>
#include <iterator>
#include <memory>
#include <string>

#include "domains.h"
#include "moptions.h"
#include "serviceparser.h"

#include "tools/strutil.h"

namespace fwm {

	class ServiceAny : public Service
	{
	public:
		ServiceAny() :
			Service("any", Protocol::any(), Ports::any())
		{}

		virtual bdd make_bdd() const override
		{
			return bddtrue;
		}
	};


	Service* Service::create(const std::string& name, const std::string& proto_port)
	{
		Service* service{ nullptr };

		if (proto_port == "any")
			service = new ServiceAny();

		else {
			auto range = parse_protocol_port(proto_port);
			switch (std::get<0>(range)) {
			case ProtocolType::TCP:
				service = new TcpService(name, std::get<1>(range), std::get<2>(range));
				break;

			case ProtocolType::UDP:
				service = new UdpService(name, std::get<1>(range), std::get<2>(range));
				break;

			case ProtocolType::ICMP:
				service = new IcmpService(name, std::get<1>(range), std::get<2>(range));
				break;

			default:
				throw ServiceError(proto_port);
			}
		}

		return service;
	}


	Service* Service::any()
	{
		return new ServiceAny();
	}


	Service::Service(const std::string& name, const Protocol* protocol, const Ports* ports) :
		NamedMnode(name, ModelOptions::empty()),
		_protocol{ std::unique_ptr<const Protocol>(protocol) },
		_ports{ std::unique_ptr<const Ports>(ports) }
	{
	}


	bdd Service::make_bdd() const
	{
		return _protocol->make_bdd() & _ports->make_bdd();
	}


	std::string Service::to_string() const
	{
		return is_any()
			? name()
			: _protocol->to_string() + "/" + _ports->to_string();
	}


	ServiceGroup::ServiceGroup(const std::string& name) :
		Group<Service>(name)
	{
	}


	ServiceGroup::ServiceGroup(const std::string& name, const Service* service) :
		Group<Service>(name, service)
	{
	}


	bool ServiceGroup::is_app_services() const
	{
		return false;
	}


	TcpService::TcpService(const std::string& name, uint32_t lower_port, uint32_t upper_port) :
		Service(
			name,
			new TcpProtocol(),
			new TcpPorts(lower_port, upper_port))
	{
	}


	UdpService::UdpService(const std::string& name, uint32_t lower_port, uint32_t upper_port) :
		Service(
			name,
			new UdpProtocol(),
			new UdpPorts(lower_port, upper_port))
	{
	}


	IcmpService::IcmpService(const std::string& name, uint32_t type) :
		IcmpService(name, type, type)
	{
	}


	IcmpService::IcmpService(const std::string& name, uint32_t lower_port, uint32_t upper_port) :
		Service(
			name,
			new IcmpProtocol(),
			new IcmpPorts(lower_port, upper_port))
	{
	}

}
