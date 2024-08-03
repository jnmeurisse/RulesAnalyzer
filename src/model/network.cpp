/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#include "network.h"

#include <exception>
#include <memory>

#include "domains.h"


namespace fwm {

	Network::Network() :
		_firewalls{},
		_src_zone_cache{},
		_dst_zone_cache{},
		_src_addr_cache{},
		_src_addr_group_cache{},
		_dst_addr_cache{},
		_dst_addr_group_cache{},
		_svc_cache{},
		_svc_group_cache{},
		_app_cache{ ApplicationCache(), ApplicationCache() },
		_app_group_cache{ ApplicationGroupCache(), ApplicationGroupCache() },
		_appsvc_cache{},
		_zones{},
		_app_id{ 1 },
		_user_id{ 1 }
	{
		_src_zone_cache.set("any", SrcZone::any());
		_dst_zone_cache.set("any", DstZone::any());
		_src_addr_cache.set("any", SrcAddress::any());
		_dst_addr_cache.set("any", DstAddress::any());
		_svc_cache.set("any", Service::any());
		_app_cache[0].set("any", Application::any());
		_app_cache[1].set("any", Application::any());
		_appsvc_cache.set("any", Service::any());
		_user_cache.set("any", User::any());

		model_options.add(ModelOption::Application);
		model_options.add(ModelOption::User);
	}


	void Network::add(FirewallPtr& firewall)
	{
		_firewalls[firewall->name()] = std::move(firewall);
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


	Table Network::info() const
	{
		std::vector<std::string> firewall_names;

		for (auto iter = _firewalls.cbegin(); iter != _firewalls.cend(); iter++) {
			firewall_names.push_back(iter->first);
		}
		std::sort(firewall_names.begin(), firewall_names.end());

		Table table{ { "name", "rules" } };
		Row *row;

		for (const std::string& name : firewall_names) {
			row = table.add_row();
			row->cell(0).append(name);
			row->cell(1).append(_firewalls.at(name)->acl().size());
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


	const SrcZone* Network::register_src_zone(const std::string& name)
	{
		const SrcZone* zone{ get_src_zone(name) };

		if (!zone) {
			const uint32_t zone_id = get_zone_id(name);
			if (zone_id > Domains::get(DomainType::SrcZone).upper())
				throw std::runtime_error("source zone id overflow");

			// Create and register a new source zone
			zone = _src_zone_cache.set(
				name, 
				SrcZone::create(name, zone_id)
			);
		}

		return zone;
	}


	const DstZone* Network::register_dst_zone(const std::string& name)
	{
		const DstZone* zone { get_dst_zone(name) };

		if (!zone) {
			const uint32_t zone_id = get_zone_id(name);
			if (zone_id > Domains::get(DomainType::DstZone).upper())
				throw std::runtime_error("destination zone id overflow");

			// Create and register a new destination zone
			zone = _dst_zone_cache.set(
				name, 
				DstZone::create(name, zone_id)
			);
		}

		return zone;
	}


	const SrcAddress* Network::register_src_address(const std::string& name, const std::string& address)
	{
		const SrcAddress* src_address{ get_src_address(name) };

		if (!src_address) {
			// Create and register a new source address
			src_address = _src_addr_cache.set(
				name, 
				SrcAddress::create(name, address, false)
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
			src_addresses = _src_addr_group_cache.set(name, group);

			// Add addresses to this group
			for (int idx = 0; idx < addresses.size(); idx++) {
				const std::string indexed_address_name{ string_format("%s[%d]", name.c_str(), idx) };
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
			src_address_group  = _src_addr_group_cache.set(name, group);

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
				name, 
				DstAddress::create(name, address, false)
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
			dst_addresses= _dst_addr_group_cache.set(name, group);

			// Add addresses to this group
			for (int idx = 0; idx < addresses.size(); idx++) {
				const std::string indexed_address_name{ string_format("%s[%d]", name.c_str(), idx) };
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
			dst_address_group = _dst_addr_group_cache.set(name, group);

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
			service = _svc_cache.set(
				 name, 
				 Service::create(name, service_definition)
			 );
		}

		return service;
	}


	const ServiceGroup* Network::register_multi_service(const std::string& name, const std::vector<std::string>& service_definitions)
	{
		const ServiceGroup* services{ get_service_group(name) };

		if (!services) {
			// Create and register a new service group
			ServiceGroup* group{ new ServiceGroup(name) };
			_svc_group_cache.set(name, group);

			// Add services to this group
			for (int idx = 0; idx < service_definitions.size(); idx++) {
				const std::string indexed_service_name{ string_format("%s[%d]", name.c_str(), idx) };
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
			_svc_group_cache.set(name, group);

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
			// This application is not yet registered.
			if (_app_id >= Domains::get(DomainType::Application).upper())
				throw std::runtime_error("application id overflow");

			// Register all default services for this application.
			ServiceGroupPtr service_group{ new ServiceGroup("$appsvc") };
			for (const std::string& service : service_definitions) {
				service_group->add_member(register_appsvc(service, service));
			}

			// Create and register this new application.
			application = _app_cache[use_app_svc].set(
				name,
				Application::create(
					name,
					_app_id++,
					service_group,
					model_options,
					use_app_svc
				)
			);
		}

		return application;
	}


	const ApplicationGroup * Network::register_application_group(const std::string& name, const std::vector<std::string>& members, bool use_app_svc)
	{
		const ApplicationGroup* application_group{ get_application_group(name, use_app_svc) };

		if (!application_group) {
			// This application group is not yet registered.

			// Create a new empty application and register it.
			ApplicationGroup* group{ new ApplicationGroup(name) };
			_app_group_cache[use_app_svc].set(name, group);

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
			if (_user_id >= Domains::get(DomainType::User).upper())
				throw std::runtime_error("user id overflow");

			// Create and register a new user
			user= _user_cache.set(
				name, 
				User::create(name, _user_id++, model_options)
			);
		}

		return user;
	}


	const UserGroup* Network::register_user_group(const std::string& name, const std::vector<std::string>& members)
	{
		const UserGroup* user_group{ get_user_group(name) };

		if (!user_group) {
			// Create and register a new user group
			UserGroup* group{ new UserGroup(name) };
			_user_group_cache.set(name, group);

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


	const Service* Network::register_appsvc(const std::string& name, const std::string& service_definition)
	{
		const Service* service{ _appsvc_cache.get(name) };
		
		if (!service) {
			service = _appsvc_cache.set(
				name, 
				Service::create(name, service_definition)
			);
		}

		return service;
	}


	uint32_t Network::get_zone_id(const std::string& name)
	{
		auto it = std::find(_zones.begin(), _zones.end(), name);
		size_t index;

		// the zone id is the index+1 of the name in the vector.
		if (it != _zones.end()) {
			// zone name was found, calculating the index.
			index = it - _zones.begin() + 1;
		}
		else {
			// zone name not found, add the name to the vector.
			_zones.push_back(name);
			index = _zones.size();
		}

		return static_cast<uint32_t>(index);
	}

}
