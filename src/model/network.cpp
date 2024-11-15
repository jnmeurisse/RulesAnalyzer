/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#include "model/network.h"

#include <cstdint>
#include <cassert>
#include <memory>
#include <stdexcept>
#include <utility>
#include "model/domains.h"
#include "model/mconfig.h"
#include "model/range.h"
#include "fmt/core.h"


namespace fwm {

	Network::Network(const ModelConfig& config) :
		_model_config{ config },
		_firewalls{},
		_src_zone_cache{},
		_dst_zone_cache{},
		_src_addr_cache{},
		_src_addr_group_cache{},
		_dst_addr_cache{},
		_dst_addr_group_cache{},
		_svc_cache{},
		_svc_group_cache{},
		_app_cache{ Cache<Application>(), Cache<Application>() },
		_app_group_cache{ Cache<ApplicationGroup>(), Cache<ApplicationGroup>() },
		_appsvc_cache{},
		_user_cache{},
		_user_group_cache{},
		_url_cache{},
		_url_group_cache{},
		_zone_id_gen{ "zone",  ZoneDomain::max()},
		_app_id_gen{ "application", ApplicationDomain::max() },
		_user_id_gen{ "user", UserDomain::max() },
		_url_id_gen{ "url", UrlDomain::max() }
	{
		_src_zone_cache.set(SrcZone::any());
		_dst_zone_cache.set(DstZone::any());

		switch (_model_config.ip_model) {
		case IPAddressModel::IP4Model:
			_src_addr_cache.set(SrcAddress::any4(IPAddressModel::IP4Model));
			_dst_addr_cache.set(DstAddress::any4(IPAddressModel::IP4Model));
			break;

		case IPAddressModel::IP6Model:
			_src_addr_cache.set(SrcAddress::any6(IPAddressModel::IP6Model));
			_dst_addr_cache.set(DstAddress::any6(IPAddressModel::IP6Model));
			break;

		default:
			_src_addr_cache.set(SrcAddress::any4(IPAddressModel::IP64Model));
			_src_addr_cache.set(SrcAddress::any6(IPAddressModel::IP64Model));

			_dst_addr_cache.set(DstAddress::any4(IPAddressModel::IP64Model));
			_dst_addr_cache.set(DstAddress::any6(IPAddressModel::IP64Model));
		}

		_svc_cache.set(Service::any());
		_app_cache[0].set(Application::any());
		_app_cache[1].set(Application::any());
		_appsvc_cache.set(Service::any());
		_user_cache.set(User::any());
		_url_cache.set(Url::any());

		model_options.add(ModelOption::Application);
		model_options.add(ModelOption::User);
		model_options.add(ModelOption::Url);
	}


	const ModelConfig& Network::config() const
	{
		return _model_config;
	}


	Firewall& Network::add(Firewall* firewall)
	{
		assert(firewall);
		assert(&firewall->network() == this);

		_firewalls[firewall->name()] = std::unique_ptr<Firewall>(firewall);
		return *_firewalls[firewall->name()];
	}


	void Network::del(const std::string& name)
	{
		_firewalls.erase(name);
	}


	Firewall* Network::get(const std::string& name) const
	{
		auto it = _firewalls.find(name);

		if (it == _firewalls.end())
			return nullptr;
		else
			return it->second.get();
	}


	Table Network::create_info_table() const
	{
		std::vector<std::string> firewall_names;

		for (auto iter = _firewalls.cbegin(); iter != _firewalls.cend(); iter++) {
			firewall_names.push_back(iter->first);
		}
		std::sort(firewall_names.begin(), firewall_names.end());

		Table table{ { "name", "rules" } };

		for (const std::string& name : firewall_names) {
			Row& row = table.add_row();
			row.cell(0).append(name);
			row.cell(1).append(_firewalls.at(name)->acl().size());
		}

		return table;
	}


	const SrcZone* Network::get_src_zone(const std::string& name) const
	{
		return _src_zone_cache.get(name);
	}


	const DstZone* Network::get_dst_zone(const std::string& name) const
	{
		return _dst_zone_cache.get(name);
	}


	const SrcAddress* Network::get_src_address(const std::string& name) const
	{
		return _src_addr_cache.get(name);
	}


	const SrcAddressGroup* Network::get_src_address_group(const std::string& name) const
	{
		return _src_addr_group_cache.get(name);
	}


	const DstAddress* Network::get_dst_address(const std::string& name) const
	{
		return _dst_addr_cache.get(name);
	}


	const DstAddressGroup* Network::get_dst_address_group(const std::string& name) const
	{
		return _dst_addr_group_cache.get(name);
	}


	const Service* Network::get_service(const std::string& name) const
	{
		return _svc_cache.get(name);
	}


	const ServiceGroup* Network::get_service_group(const std::string& name) const
	{
		return _svc_group_cache.get(name);
	}


	const Application* Network::get_application(const std::string& name, bool use_app_svc) const
	{
		return _app_cache[use_app_svc].get(name);
	}


	const Application* Network::get_application(const std::string& name) const
	{
		const Application* application{ get_application(name, true) };

		if (application)
			return application;
		else
			return get_application(name, false);
	}


	const ApplicationGroup* Network::get_application_group(const std::string& name, bool use_app_svc) const
	{
		return _app_group_cache[use_app_svc].get(name);
	}


	const ApplicationGroup* Network::get_application_group(const std::string& name) const
	{
		const ApplicationGroup* group{ get_application_group(name, true) };

		if (group)
			return group;
		else
			return get_application_group(name, false);
	}


	const User* Network::get_user(const std::string& name) const
	{
		return _user_cache.get(name);
	}


	const UserGroup* Network::get_user_group(const std::string& name) const
	{
		return _user_group_cache.get(name);
	}


	const Url* Network::get_url(const std::string& name) const
	{
		return _url_cache.get(name);
	}


	const UrlGroup* Network::get_url_group(const std::string& name) const
	{
		return _url_group_cache.get(name);
	}


	const SrcZone* Network::register_src_zone(const std::string& name)
	{
		const SrcZone* zone{ get_src_zone(name) };

		if (!zone) {
			const uint32_t zone_id = _zone_id_gen.get_id(name);

			// Create and register a new source zone
			zone = _src_zone_cache.set(SrcZone::create(name, zone_id));
		}

		return zone;
	}


	const DstZone* Network::register_dst_zone(const std::string& name)
	{
		const DstZone* zone { get_dst_zone(name) };

		if (!zone) {
			const uint32_t zone_id = _zone_id_gen.get_id(name);

			// Create and register a new destination zone
			zone = _dst_zone_cache.set(DstZone::create(name, zone_id));
		}

		return zone;
	}


	const SrcAddress* Network::register_src_address(const std::string& name, const std::string& address)
	{
		const SrcAddress* src_address{ get_src_address(name) };

		if (!src_address) {
			// Create and register a new source address
			src_address = _src_addr_cache.set(
				SrcAddress::create(name, address, _model_config.ip_model, _model_config.strict_ip_parser)
			);
		}

		return src_address;
	}


	const SrcAddressGroup* Network::register_src_multi_address(const std::string& name, const std::vector<std::string>& addresses)
	{
		const SrcAddressGroup* src_addresses{ get_src_address_group(name) };

		if (!src_addresses) {
			// Create and register a new address group
			SrcAddressGroup* group{ new SrcAddressGroup(name) };
			src_addresses = _src_addr_group_cache.set(group);

			// Add addresses to this group
			for (int idx = 0; idx < addresses.size(); idx++) {
				const std::string indexed_address_name{ fmt::format("{}[{}]", name, idx) };
				group->add_member(register_src_address(indexed_address_name, addresses[idx]));
			}
		}

		return src_addresses;
	}


	const SrcAddressGroup* Network::register_src_address_group(const std::string& name, const std::vector<std::string>& members)
	{
		const SrcAddressGroup* src_address_group{ get_src_address_group(name) };

		if (!src_address_group) {
			// Create and register a new address group
			SrcAddressGroup* group{ new SrcAddressGroup(name) };
			src_address_group  = _src_addr_group_cache.set(group);

			// Add all given members to this source address group.  It is assumed
			// that members have been registered before calling this method.
			for (const std::string& member : members) {
				const SrcAddress* address{ get_src_address(member) };
				if (address) {
					group->add_member(address);
				}
				else {
					const SrcAddressGroup* sub_group{ get_src_address_group(member) };
					if (sub_group)
						group->add_member(sub_group);
					else
						;  // ignore missing member
				}
			}
		}

		return src_address_group;
	}


	const DstAddress* Network::register_dst_address(const std::string& name, const std::string& address)
	{
		const DstAddress* dst_address{ get_dst_address(name) };

		if (!dst_address) {
			// Create and register a new destination address
			dst_address = _dst_addr_cache.set(
				DstAddress::create(name, address, _model_config.ip_model, _model_config.strict_ip_parser)
			);
		}

		return dst_address;
	}


	const DstAddressGroup* Network::register_dst_multi_address(const std::string& name, const std::vector<std::string>& addresses)
	{
		const DstAddressGroup* dst_addresses{ get_dst_address_group(name) };

		if (!dst_addresses) {
			// Create and register a new address group
			DstAddressGroup* group{ new DstAddressGroup(name) };
			dst_addresses= _dst_addr_group_cache.set(group);

			// Add addresses to this group
			for (int idx = 0; idx < addresses.size(); idx++) {
				const std::string indexed_address_name{ fmt::format("{}[{}]", name, idx) };
				group->add_member(register_dst_address(indexed_address_name, addresses[idx]));
			}
		}

		return dst_addresses;
	}


	const DstAddressGroup* Network::register_dst_address_group(const std::string& name, const std::vector<std::string>& members)
	{
		const DstAddressGroup* dst_address_group{ get_dst_address_group(name) };

		if (!dst_address_group) {
			// Create and register a new address group
			DstAddressGroup* group{ new DstAddressGroup(name) };
			dst_address_group = _dst_addr_group_cache.set(group);

			// Add all given members to this destination address group.  It is assumed
			// that members have been registered before calling this method.
			for (const std::string& member : members) {
				const DstAddress* address{ get_dst_address(member) };
				if (address) {
					group->add_member(address);
				}
				else {
					const DstAddressGroup* sub_group{ get_dst_address_group(member) };
					if (sub_group)
						group->add_member(sub_group);
					else
						;  // ignore missing member
				}
			}
		}

		return dst_address_group;
	}


	const Service* Network::register_service(const std::string& name, const std::string& service_definition)
	{
		const Service* service{ get_service(name) };

		if (!service) {
			// Create and register a new service
			service = _svc_cache.set(Service::create(name, service_definition));
		}

		return service;
	}


	const ServiceGroup* Network::register_multi_service(const std::string& name, const std::vector<std::string>& service_definitions)
	{
		const ServiceGroup* services{ get_service_group(name) };

		if (!services) {
			// Create and register a new service group
			ServiceGroup* group{ new ServiceGroup(name) };
			_svc_group_cache.set(group);

			// Add services to this group
			for (int idx = 0; idx < service_definitions.size(); idx++) {
				const std::string indexed_service_name{ fmt::format("{}[{}]", name, idx) };
				group->add_member(register_service(indexed_service_name, service_definitions[idx]));
			}

			services = group;
		}

		return services;
	}


	const ServiceGroup* Network::register_service_group(const std::string& name, const std::vector<std::string>& members)
	{
		const ServiceGroup* service_group{ get_service_group(name) };

		if (!service_group) {
			// Create and register a new service group
			ServiceGroup* group{ new ServiceGroup(name) };
			_svc_group_cache.set(group);

			// Add all given members to this service group.  It is assumed that members
			// have been registered before calling this method.
			for (const std::string& member : members) {
				const Service* service{ get_service(member) };
				if (service) {
					group->add_member(service);
				}
				else {
					const ServiceGroup* sub_group{ get_service_group(member) };
					if (sub_group)
						group->add_member(sub_group);
					else
						;  // ignore missing member
				}
			}

			service_group = group;
		}

		return service_group;
	}


	const Application* Network::register_application(const std::string& name, const std::vector<std::string>& service_definitions, bool use_app_svc)
	{
		const Application* application{ get_application(name, use_app_svc) };

		if (!application) {
			const uint32_t app_id = _app_id_gen.get_id(name);

			// Register all default services for this application.
			ServiceGroupPtr service_group{ new ServiceGroup("$appsvc") };
			for (const std::string& service : service_definitions) {
				service_group->add_member(register_appsvc(service, service));
			}

			// Create and register this new application.
			application = _app_cache[use_app_svc].set(
				Application::create(name, app_id, service_group.release(), model_options, use_app_svc)
			);
		}

		return application;
	}


	const ApplicationGroup* Network::register_application_group(const std::string& name, const std::vector<std::string>& members, bool use_app_svc)
	{
		const ApplicationGroup* application_group{ get_application_group(name, use_app_svc) };

		if (!application_group) {
			// This application group is not yet registered.

			// Create a new empty application and register it.
			ApplicationGroup* group{ new ApplicationGroup(name) };
			_app_group_cache[use_app_svc].set(group);

			// Add all given members to this application group.  It is assumed that members
			// have been registered before calling this method.
			for (const std::string& member : members) {
				const Application* application{ get_application(member, use_app_svc) };
				if (application) {
					group->add_member(application);
				}
				else {
					const ApplicationGroup* sub_group{ get_application_group(member, use_app_svc) };
					if (sub_group)
						group->add_member(sub_group);
					else
						;  // ignore a missing member
				}
			}

			application_group = group;
		}

		return application_group;
	}


	const User* Network::register_user(const std::string& name)
	{
		const User* user{ get_user(name) };

		if (!user) {
			const int32_t user_id = _user_id_gen.get_id(name);

			// Create and register a new user
			user= _user_cache.set(User::create(name, user_id, model_options));
		}

		return user;
	}


	const UserGroup* Network::register_user_group(const std::string& name, const std::vector<std::string>& members)
	{
		const UserGroup* user_group{ get_user_group(name) };

		if (!user_group) {
			// Create and register a new user group
			UserGroup* group{ new UserGroup(name) };
			_user_group_cache.set(group);

			// Add all given members to this user group.  It is assumed that members
			// have been registered before calling this method.
			for (const std::string& member : members) {
				const User* user{ get_user(member) };
				if (user) {
					group->add_member(user);
				}
				else {
					const UserGroup* sub_group{ get_user_group(member) };
					if (sub_group)
						group->add_member(sub_group);
					else
						;  // ignore missing member
				}
			}

			user_group = group;
		}

		return user_group;
	}


	const Url* Network::register_url(const std::string& name)
	{
		const Url* url{ get_url(name) };

		if (!url) {
			const int32_t url_id = _url_id_gen.get_id(name);

			// Create and register a new url
			url = _url_cache.set(Url::create(name, url_id, model_options));
		}

		return url;
	}


	const UrlGroup* Network::register_url_group(const std::string& name, const std::vector<std::string>& members)
	{
		const UrlGroup* url_group{ get_url_group(name) };

		if (!url_group) {
			// Create and register a new url group
			UrlGroup* group{ new UrlGroup(name) };
			_url_group_cache.set(group);

			// Add all given members to this url group.  It is assumed that members
			// have been registered before calling this method.
			for (const std::string& member : members) {
				const Url* url{ get_url(member) };
				if (url) {
					group->add_member(url);
				}
				else {
					const UrlGroup* sub_group{ get_url_group(member) };
					if (sub_group)
						group->add_member(sub_group);
					else
						;  // ignore missing member
				}
			}

			url_group = group;
		}

		return url_group;
	}


	const Service* Network::register_appsvc(const std::string& name, const std::string& service_definition)
	{
		const Service* service{ _appsvc_cache.get(name) };

		if (!service) {
			service = _appsvc_cache.set(Service::create(name, service_definition));
		}

		return service;
	}

}
