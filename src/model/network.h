/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#pragma once
#include "global.h"

#include <list>
#include <map>
#include <memory>
#include <string>

#include "model/application.h"
#include "model/cache.h"
#include "model/ipaddress.h"
#include "model/idgen.h"
#include "model/address.h"
#include "model/firewall.h"
#include "model/mconfig.h"
#include "model/rule.h"
#include "model/service.h"
#include "model/table.h"
#include "model/user.h"
#include "model/url.h"
#include "model/zone.h"


namespace fwm {

	class Network
	{
	public:
		Network(const ModelConfig& config);

		/* Returns the model configuration used on this network.
		*/
		const ModelConfig& config() const;

		/* Adds a new firewall to this network.
		*/
		void add(Firewall* firewall);

		/* Deletes a firewall from this network.
		*/
		void del(const std::string& name);

		/* Returns a firewall from this network.  The method returns
		 * a null pointer if the firewall name does not exist.
		 */
		Firewall* get(const std::string& name) const;

		/* Returns a table showing the list of firewalls defined on this network.
		*/
		Table create_info_table() const;

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

		const Url* get_url(const std::string& name) const;
		const UrlGroup* get_url_group(const std::string& name) const;

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

		const Url* register_url(const std::string& name);
		const UrlGroup* register_url_group(const std::string& name, const std::vector<std::string>& members);

		ModelOptions model_options;

	private:
		const ModelConfig _model_config;

		// All firewalls
		std::map<std::string, FirewallPtr> _firewalls;

		//
		// All caches.
		// The definitions of objects used by all firewalls are stored there.

		// Zones caches
		Cache<SrcZone> _src_zone_cache;
		Cache<DstZone> _dst_zone_cache;

		// Addresses caches
		Cache<SrcAddress> _src_addr_cache;
		Cache<SrcAddressGroup> _src_addr_group_cache;
		Cache<DstAddress> _dst_addr_cache;
		Cache<DstAddressGroup> _dst_addr_group_cache;

		// Services caches
		Cache<Service> _svc_cache;
		Cache<ServiceGroup> _svc_group_cache;

		// Applications caches.  An application that allows the traffic only on
		// their default services is stored in _app_cache[1] while an application that
		// allows the traffic on any port is stored in _app_cache[0].  Same for groups
		// of applications.  The _appsvc_cache stores the service definition shared
		// by all applications.
		Cache<Application> _app_cache[2];
		Cache<ApplicationGroup> _app_group_cache[2];
		Cache<Service> _appsvc_cache;

		// Users caches.
		Cache<User> _user_cache;
		Cache<UserGroup> _user_group_cache;

		// Urls caches.
		Cache<Url> _url_cache;
		Cache<UrlGroup> _url_group_cache;


		// Dynamic id allocated to a zone, an application, url or new user when we create
		// and store such object in a cache.
		IdGenerator _zone_id_gen;
		IdGenerator _app_id_gen;
		IdGenerator _user_id_gen;
		IdGenerator _url_id_gen;

		const Service* register_appsvc(const std::string& name, const std::string& service_definition);
	};

}
