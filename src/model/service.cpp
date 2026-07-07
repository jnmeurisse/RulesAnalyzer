/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#include "service.h"

#include <memory>

#include "model/domains.h"
#include "model/moptions.h"
#include "model/serviceparser.h"


namespace fwm {

	class ServiceAny final : public Service
	{
	public:
		ServiceAny() :
			Service("any", Protocol::any(), Ports::any())
		{}


		virtual bdd make_bdd() const override
		{
			return bddtrue;
		}


		virtual std::string to_string() const override
		{
			return "any";
		}
	};


	Service::Ptr Service::create(const std::string& name, const std::string& proto_port)
	{
		Service* service{ nullptr };

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

		return Service::Ptr(service);
	}


	Service::Ptr Service::any()
	{
		return Service::Ptr(new ServiceAny());
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
		return _ports->is_any()
			? _protocol->to_string()
			: _protocol->to_string() + "/" + _ports->to_string();
	}


	static bool compare_services(const Service& s1, const Service& s2)
	{
		if (s1.is_any())
			return false;
		else if (s1.protocol().pt() < s2.protocol().pt())
			return false;
		else if (s1.protocol().equal(s2.protocol()))
			return s1.compare_name(s2) < 0;
		else
			return true;
	}



	ServiceList& ServiceList::sort()
	{
		auto cmp = [](const Service* service1, const Service* service2) -> bool {
				return compare_services(*service1, *service2);
		};

		NamedMnodeList<Service>::sort(cmp);

		return *this;
	}


	bool ServiceGroup::is_app_services() const
	{
		return false;
	}


	AnyServiceGroup::AnyServiceGroup() :
		ServiceGroup("$any-svc-group", Service::any())
	{
	}


	bdd AnyServiceGroup::make_bdd() const
	{
		return bddtrue;
	}


	TcpService::TcpService(const std::string& name, const Ports* ports) :
		Service(name, new TcpProtocol(), ports)
	{
	}


	TcpService::TcpService(const std::string& name, uint16_t lower_port, uint16_t upper_port) :
		TcpService(name, new TcpPorts(lower_port, upper_port))
	{
	}


	UdpService::UdpService(const std::string& name, const Ports* ports) :
		Service(name, new UdpProtocol(), ports)
	{
	}


	UdpService::UdpService(const std::string& name, uint16_t lower_port, uint16_t upper_port) :
		UdpService(name, new UdpPorts(lower_port, upper_port))
	{
	}


	IcmpService::IcmpService(const std::string& name, const Ports* ports) :
		Service(name, new IcmpProtocol(), ports)
	{
	}


	IcmpService::IcmpService(const std::string& name, uint16_t lower_port, uint16_t upper_port) :
		IcmpService(name, new IcmpPorts(lower_port, upper_port))
	{
	}


	IcmpService::IcmpService(const std::string& name, uint16_t type) :
		IcmpService(name, type, type)
	{
	}

}

