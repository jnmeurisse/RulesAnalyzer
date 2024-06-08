/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#pragma once
#include <memory>
#include <map>
#include <list>

#include "application.h"
#include "applicationcache.h"
#include "address.h"
#include "addresscache.h"
#include "firewall.h"
#include "rule.h"
#include "service.h"
#include "servicecache.h"
#include "user.h"
#include "usercache.h"
#include "zone.h"
#include "zonecache.h"

namespace fwm {

	class Network
	{
	public:
		Network();

		/* Adds a new firewall to this network.
		*/
		void add(FirewallPtr& firewall);

		/* Deletes a firewall from this network.
		*/
		void del(const std::string& name);

		/* Returns a firewall from this network.  The method returns
		 * a null pointer if the firewall name does not exist.
		 */
		Firewall* get(const std::string& name);


		const SrcZone* get_src_zone(const std::string& name) const;
		const DstZone* get_dst_zone(const std::string& name) const;

		const SrcAddress* get_src_address(const std::string& name) const;
		const SrcAddressGroup* get_src_address_group(const std::string& name) const;

		const DstAddress* get_dst_address(const std::string& name) const;
		const DstAddressGroup* get_dst_address_group(const std::string& name) const;

		const Service* get_service(const std::string& name) const;
		const ServiceGroup* get_service_group(const std::string& name) const;

		const Application* get_application(const std::string& name, bool use_app_svc) const;
		const Application* get_application(const std::string& name) const;
		const ApplicationGroup* get_application_group(const std::string& name, bool use_app_svc) const;
		const ApplicationGroup* get_application_group(const std::string& name) const;

		const User* get_user(const std::string& name) const;
		const UserGroup* get_user_group(const std::string& name) const;

		const SrcZone* register_src_zone(const std::string& name);
		const DstZone* register_dst_zone(const std::string& name);

		const SrcAddress* register_src_address(const std::string& name, const std::string& address);
		const SrcAddressGroup* register_src_multi_address(const std::string& name, const std::vector<std::string>& addresses);
		const SrcAddressGroup* register_src_address_group(const std::string& name, const std::vector<std::string>& members);

		const DstAddress* register_dst_address(const std::string& name, const std::string& address);
		const DstAddressGroup* register_dst_multi_address(const std::string& name, const std::vector<std::string>& addresses);
		const DstAddressGroup* register_dst_address_group(const std::string& name, const std::vector<std::string>& members);

		const Service* register_service(const std::string& name, const std::string& service_definition);
		const ServiceGroup* register_multi_service(const std::string& name, const std::vector<std::string>& service_definitions);
		const ServiceGroup* register_service_group(const std::string& name, const std::vector<std::string>& members);

		const Application* register_application(const std::string& name, const std::vector<std::string>& service_definitions, bool use_app_svc);
		const ApplicationGroup* register_application_group(const std::string& name, const std::vector<std::string>& members, bool use_app_svc);

		const User* register_user(const std::string& name);
		const UserGroup* register_user_group(const std::string& name, const std::vector<std::string>& members);

		ModelOptions model_options;

	private:
		// All firewalls
		std::map<std::string, FirewallPtr> _firewalls;

		//
		// All caches.
		// The definitions of objects used by all firewalls are stored there.

		// Zones caches
		SrcZoneCache _src_zone_cache;
		DstZoneCache _dst_zone_cache;

		// Addresses caches
		SrcAddressCache _src_addr_cache;
		SrcAddressGroupCache _src_addr_group_cache;
		DstAddressCache _dst_addr_cache;
		DstAddressGroupCache _dst_addr_group_cache;

		// Services caches
		ServiceCache _svc_cache;
		ServiceGroupCache _svc_group_cache;

		// Applications caches.  An application that allows the traffic only on
		// their default services is stored in _app_cache[1] while an application that 
		// allows the traffic on any port is stored in _app_cache[0].  Same of groups
		// of applications.  The _appsvc_cache stores the service definition shared
		// by all applications.
		ApplicationCache _app_cache[2];
		ApplicationGroupCache _app_group_cache[2];
		ServiceCache _appsvc_cache;

		// Users caches.
		UserCache _user_cache;
		UserGroupCache _user_group_cache;

		// Zone names.  A zone id is the index+1 of the zone name in this vector.
		std::vector<std::string> _zones;

		// Dynamic id allocated to an application or new user when we create
		// and store such object in a cache.
		uint32_t _app_id;
		uint32_t _user_id;

		const Service* register_appsvc(const std::string& name, const std::string& service_definition);
		uint32_t get_zone_id(const std::string& name);
	};

}
