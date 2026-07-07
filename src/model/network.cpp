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


	SrcZonePtr Network::get_src_zone(const std::string& name) const
	{
		return _src_zone_cache.get(name);
	}


	DstZonePtr Network::get_dst_zone(const std::string& name) const
	{
		return _dst_zone_cache.get(name);
	}


	SrcAddressPtr Network::get_src_address(const std::string& name) const
	{
		return _src_addr_cache.get(name);
	}


	SrcAddressGroupPtr Network::get_src_address_group(const std::string& name) const
	{
		return _src_addr_group_cache.get(name);
	}


	DstAddressPtr Network::get_dst_address(const std::string& name) const
	{
		return _dst_addr_cache.get(name);
	}


	DstAddressGroupPtr Network::get_dst_address_group(const std::string& name) const
	{
		return _dst_addr_group_cache.get(name);
	}


	ServicePtr Network::get_service(const std::string& name) const
	{
		return _svc_cache.get(name);
	}


	ServiceGroupPtr Network::get_service_group(const std::string& name) const
	{
		return _svc_group_cache.get(name);
	}


	ApplicationPtr Network::get_application(const std::string& name, bool use_app_svc) const
	{
		return _app_cache[use_app_svc].get(name);
	}


	ApplicationPtr Network::get_application(const std::string& name) const
	{
		ApplicationPtr application{ get_application(name, true) };

		if (application)
			return application;
		else
			return get_application(name, false);
	}


	ApplicationGroupPtr Network::get_application_group(const std::string& name, bool use_app_svc) const
	{
		return _app_group_cache[use_app_svc].get(name);
	}


	ApplicationGroupPtr Network::get_application_group(const std::string& name) const
	{
		ApplicationGroupPtr group{ get_application_group(name, true) };

		if (group)
			return group;
		else
			return get_application_group(name, false);
	}


	UserPtr Network::get_user(const std::string& name) const
	{
		return _user_cache.get(name);
	}


	UserGroupPtr Network::get_user_group(const std::string& name) const
	{
		return _user_group_cache.get(name);
	}


	UrlPtr Network::get_url(const std::string& name) const
	{
		return _url_cache.get(name);
	}


	UrlGroupPtr Network::get_url_group(const std::string& name) const
	{
		return _url_group_cache.get(name);
	}


	SrcZonePtr Network::register_src_zone(const std::string& name)
	{
		SrcZonePtr zone_ptr{ get_src_zone(name) };

		if (!zone_ptr) {
			const uint32_t zone_id = _zone_id_gen.get_id(name);

			// Create and register a new source zone
			zone_ptr = _src_zone_cache.set(SrcZone::create(name, zone_id));
		}

		return zone_ptr;
	}


	DstZonePtr Network::register_dst_zone(const std::string& name)
	{
		DstZonePtr zone { get_dst_zone(name) };

		if (!zone) {
			const uint32_t zone_id = _zone_id_gen.get_id(name);

			// Create and register a new destination zone
			zone = _dst_zone_cache.set(DstZone::create(name, zone_id));
		}

		return zone;
	}


	SrcAddressPtr Network::register_src_address(const std::string& name, const std::string& address)
	{
		SrcAddressPtr src_address{ get_src_address(name) };

		if (!src_address) {
			// Create and register a new source address
			src_address = _src_addr_cache.set(
				SrcAddress::create(
					name,
					address,
					_model_config.ip_model,
					_model_config.strict_ip_parser
				)
			);
		}

		return src_address;
	}


	SrcAddressGroupPtr Network::register_src_multi_address(const std::string& name, const std::vector<std::string>& addresses)
	{
		SrcAddressGroupPtr src_addresses{ get_src_address_group(name) };

		if (!src_addresses) {
			// Create a new address group
			SrcAddressGroup* group{ new SrcAddressGroup(name) };

			// Register this new group
			src_addresses = _src_addr_group_cache.set(SrcAddressGroupPtr(group));

			// Add addresses to this group
			for (int idx = 0; idx < addresses.size(); idx++) {
				const std::string indexed_address_name{ fmt::format("{}[{}]", name, idx) };
				group->add_member(register_src_address(indexed_address_name, addresses[idx]));
			}
		}

		return src_addresses;
	}


	SrcAddressGroupPtr Network::register_src_address_group(const std::string& name, const std::vector<std::string>& members)
	{
		SrcAddressGroupPtr src_address_group{ get_src_address_group(name) };

		if (!src_address_group) {
			// Create a new address group
			SrcAddressGroup* group{ new SrcAddressGroup(name) };

			// Register this new group
			src_address_group = _src_addr_group_cache.set(SrcAddressGroupPtr(group));

			// Add all given members to this source address group.  It is assumed
			// that members have been registered before calling this method.
			for (const std::string& member : members) {
				SrcAddressPtr address{ get_src_address(member) };
				if (address) {
					group->add_member(address);
				}
				else {
					SrcAddressGroupPtr sub_group{ get_src_address_group(member) };
					if (sub_group)
						group->add_member(sub_group);
					else
						;  // ignore missing member
				}
			}

		}

		return src_address_group;
	}


	DstAddressPtr Network::register_dst_address(const std::string& name, const std::string& address)
	{
		DstAddressPtr dst_address{ get_dst_address(name) };

		if (!dst_address) {
			// Create and register a new destination address
			dst_address = _dst_addr_cache.set(
				DstAddress::create(
					name, 
					address,
					_model_config.ip_model,
					_model_config.strict_ip_parser
				)
			);
		}

		return dst_address;
	}


	DstAddressGroupPtr Network::register_dst_multi_address(const std::string& name, const std::vector<std::string>& addresses)
	{
		DstAddressGroupPtr dst_addresses{ get_dst_address_group(name) };

		if (!dst_addresses) {
			// Create a new address group
			DstAddressGroup* group{ new DstAddressGroup(name) };

			// Register this new group
			dst_addresses= _dst_addr_group_cache.set(DstAddressGroupPtr(group));

			// Add addresses to this group
			for (int idx = 0; idx < addresses.size(); idx++) {
				const std::string indexed_address_name{ fmt::format("{}[{}]", name, idx) };
				group->add_member(register_dst_address(indexed_address_name, addresses[idx]));
			}
		}

		return dst_addresses;
	}


	DstAddressGroupPtr Network::register_dst_address_group(const std::string& name, const std::vector<std::string>& members)
	{
		DstAddressGroupPtr dst_address_group{ get_dst_address_group(name) };

		if (!dst_address_group) {
			// Create a new address group
			DstAddressGroup* group{ new DstAddressGroup(name) };

			// Register this new group
			dst_address_group = _dst_addr_group_cache.set(DstAddressGroupPtr(group));

			// Add all given members to this destination address group.  It is assumed
			// that members have been registered before calling this method.
			for (const std::string& member : members) {
				DstAddressPtr address{ get_dst_address(member) };
				if (address) {
					group->add_member(address);
				}
				else {
					DstAddressGroupPtr sub_group{ get_dst_address_group(member) };
					if (sub_group)
						group->add_member(sub_group);
					else
						;  // ignore missing member
				}
			}
		}

		return dst_address_group;
	}


	ServicePtr Network::register_service(const std::string& name, const std::string& service_definition)
	{
		ServicePtr service{ get_service(name) };

		if (!service) {
			// Create and register a new service
			service = _svc_cache.set(
				Service::create(
					name,
					service_definition
				)
			);
		}

		return service;
	}


	ServiceGroupPtr Network::register_multi_service(const std::string& name, const std::vector<std::string>& service_definitions)
	{
		ServiceGroupPtr services{ get_service_group(name) };

		if (!services) {
			// Create a new service group
			ServiceGroup* group{ new ServiceGroup(name) };

			// Register this new group
			services = _svc_group_cache.set(ServiceGroupPtr(group));

			// Add services to this group
			for (int idx = 0; idx < service_definitions.size(); idx++) {
				const std::string indexed_service_name{ fmt::format("{}[{}]", name, idx) };
				group->add_member(register_service(indexed_service_name, service_definitions[idx]));
			}
		}

		return services;
	}


	ServiceGroupPtr Network::register_service_group(const std::string& name, const std::vector<std::string>& members)
	{
		ServiceGroupPtr service_group{ get_service_group(name) };

		if (!service_group) {
			// Create a new service group
			ServiceGroup* group{ new ServiceGroup(name) };

			// Register this new group
			service_group = _svc_group_cache.set(ServiceGroupPtr(group));

			// Add all given members to this service group.  It is assumed that members
			// have been registered before calling this method.
			for (const std::string& member : members) {
				ServicePtr service{ get_service(member) };
				if (service) {
					group->add_member(service);
				}
				else {
					ServiceGroupPtr sub_group{ get_service_group(member) };
					if (sub_group)
						group->add_member(sub_group);
					else
						;  // ignore missing member
				}
			}
		}

		return service_group;
	}


	ApplicationPtr Network::register_application(const std::string& name, const std::vector<std::string>& service_definitions, bool use_app_svc)
	{
		ApplicationPtr application{ get_application(name, use_app_svc) };

		if (!application) {
			const uint32_t app_id = _app_id_gen.get_id(name);

			// Register all default services for this application.
			std::unique_ptr<ServiceGroup> service_group{ new ServiceGroup("$appsvc") };
			for (const std::string& service : service_definitions) {
				service_group->add_member(register_appsvc(service, service));
			}

			// Create and register this new application.
			application = _app_cache[use_app_svc].set(
				Application::create(
					name, 
					app_id, 
					ServiceGroupPtr(service_group.release()),
					model_options, 
					use_app_svc
				)
			);
		}

		return application;
	}


	ApplicationGroupPtr Network::register_application_group(const std::string& name, const std::vector<std::string>& members, bool use_app_svc)
	{
		ApplicationGroupPtr application_group{ get_application_group(name, use_app_svc) };

		if (!application_group) {
			// Create a new application group
			ApplicationGroup* group{ new ApplicationGroup(name) };

			// Register this new group
			application_group = _app_group_cache[use_app_svc].set(ApplicationGroupPtr(group));

			// Add all given members to this application group.  It is assumed that members
			// have been registered before calling this method.
			for (const std::string& member : members) {
				ApplicationPtr application{ get_application(member, use_app_svc) };
				if (application) {
					group->add_member(application);
				}
				else {
					ApplicationGroupPtr sub_group{ get_application_group(member, use_app_svc) };
					if (sub_group)
						group->add_member(sub_group);
					else
						;  // ignore a missing member
				}
			}
		}

		return application_group;
	}


	UserPtr Network::register_user(const std::string& name)
	{
		UserPtr user{ get_user(name) };

		if (!user) {
			const int32_t user_id = _user_id_gen.get_id(name);

			// Create and register a new user
			user = _user_cache.set(
				User::create(
					name,
					user_id,
					model_options
				)
			);
		}

		return user;
	}


	UserGroupPtr Network::register_user_group(const std::string& name, const std::vector<std::string>& members)
	{
		UserGroupPtr user_group{ get_user_group(name) };

		if (!user_group) {
			// Create a new user group
			UserGroup* group{ new UserGroup(name) };

			// Register this new group
			user_group = _user_group_cache.set(UserGroupPtr(group));

			// Add all given members to this user group.  It is assumed that members
			// have been registered before calling this method.
			for (const std::string& member : members) {
				UserPtr user{ get_user(member) };
				if (user) {
					group->add_member(user);
				}
				else {
					UserGroupPtr sub_group{ get_user_group(member) };
					if (sub_group)
						group->add_member(sub_group);
					else
						;  // ignore missing member
				}
			}
		}

		return user_group;
	}


	UrlPtr Network::register_url(const std::string& name)
	{
		UrlPtr url{ get_url(name) };

		if (!url) {
			const int32_t url_id = _url_id_gen.get_id(name);

			// Create and register a new url
			url = _url_cache.set(
				Url::create(
					name, 
					url_id,
					model_options
				)
			);
		}

		return url;
	}


	UrlGroupPtr Network::register_url_group(const std::string& name, const std::vector<std::string>& members)
	{
		UrlGroupPtr url_group{ get_url_group(name) };

		if (!url_group) {
			// Create a new url group
			UrlGroup* group{ new UrlGroup(name) };

			// Register this new group
			url_group = _url_group_cache.set(UrlGroupPtr(group));

			// Add all given members to this url group.  It is assumed that members
			// have been registered before calling this method.
			for (const std::string& member : members) {
				UrlPtr url{ get_url(member) };
				if (url) {
					group->add_member(url);
				}
				else {
					UrlGroupPtr sub_group{ get_url_group(member) };
					if (sub_group)
						group->add_member(sub_group);
					else
						;  // ignore missing member
				}
			}
		}

		return url_group;
	}


	ServicePtr Network::register_appsvc(const std::string& name, const std::string& service_definition)
	{
		ServicePtr service{ _appsvc_cache.get(name) };

		if (!service) {
			service = _appsvc_cache.set(
				Service::create(
					name,
					service_definition
				)
			);
		}

		return service;
	}

}
