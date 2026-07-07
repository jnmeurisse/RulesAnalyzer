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

		/* Adds a new firewall to this network and returns a reference to the added firewall.
		*/
		Firewall& add(Firewall* firewall);

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

		SrcZonePtr get_src_zone(const std::string& name) const;
		DstZonePtr get_dst_zone(const std::string& name) const;

		SrcAddressPtr get_src_address(const std::string& name) const;
		SrcAddressGroupPtr get_src_address_group(const std::string& name) const;

		DstAddressPtr get_dst_address(const std::string& name) const;
		DstAddressGroupPtr get_dst_address_group(const std::string& name) const;

		ServicePtr get_service(const std::string& name) const;
		ServiceGroupPtr get_service_group(const std::string& name) const;

		ApplicationPtr get_application(const std::string& name, bool use_app_svc) const;
		ApplicationPtr get_application(const std::string& name) const;
		ApplicationGroupPtr get_application_group(const std::string& name, bool use_app_svc) const;
		ApplicationGroupPtr get_application_group(const std::string& name) const;

		UserPtr get_user(const std::string& name) const;
		UserGroupPtr get_user_group(const std::string& name) const;

		UrlPtr get_url(const std::string& name) const;
		UrlGroupPtr get_url_group(const std::string& name) const;

		SrcZonePtr register_src_zone(const std::string& name);
		DstZonePtr register_dst_zone(const std::string& name);

		SrcAddressPtr register_src_address(const std::string& name, const std::string& address);
		SrcAddressGroupPtr register_src_multi_address(const std::string& name, const std::vector<std::string>& addresses);
		SrcAddressGroupPtr register_src_address_group(const std::string& name, const std::vector<std::string>& members);

		DstAddressPtr register_dst_address(const std::string& name, const std::string& address);
		DstAddressGroupPtr register_dst_multi_address(const std::string& name, const std::vector<std::string>& addresses);
		DstAddressGroupPtr register_dst_address_group(const std::string& name, const std::vector<std::string>& members);

		ServicePtr register_service(const std::string& name, const std::string& service_definition);
		ServiceGroupPtr register_multi_service(const std::string& name, const std::vector<std::string>& service_definitions);
		ServiceGroupPtr register_service_group(const std::string& name, const std::vector<std::string>& members);

		ApplicationPtr register_application(const std::string& name, const std::vector<std::string>& service_definitions, bool use_app_svc);
		ApplicationGroupPtr register_application_group(const std::string& name, const std::vector<std::string>& members, bool use_app_svc);

		UserPtr register_user(const std::string& name);
		UserGroupPtr register_user_group(const std::string& name, const std::vector<std::string>& members);

		UrlPtr register_url(const std::string& name);
		UrlGroupPtr register_url_group(const std::string& name, const std::vector<std::string>& members);

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

		ServicePtr register_appsvc(const std::string& name, const std::string& service_definition);
	};

}
