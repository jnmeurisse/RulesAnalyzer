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
#include <list>

#include "model/table.h"
#include "ostore/ostoreconfig.h"
#include "ostore/addressobjects.h"
#include "ostore/addresspools.h"
#include "ostore/addressresolver.h"
#include "ostore/serviceobjects.h"
#include "ostore/servicepools.h"
#include "ostore/serviceresolver.h"
#include "ostore/applicationobjects.h"
#include "ostore/applicationpools.h"
#include "ostore/applicationresolver.h"
#include "ostore/userobjects.h"
#include "ostore/userpools.h"
#include "ostore/userresolver.h"
#include "ostore/urlobjects.h"
#include "ostore/urlpools.h"
#include "ostore/urlresolver.h"
#include "tools/csvreader.h"


namespace fos {
	using namespace fwm;

	class ObjectStore {
	public:
		ObjectStore(const OstoreConfig& config);

		void initialize();
		void terminate();

		void clear();
		Table info();

		int load_addresses(csv::CsvReader& reader);
		int load_address_pools(csv::CsvReader& reader);
		int load_services(csv::CsvReader& reader);
		int load_service_groups(csv::CsvReader& reader);
		int load_apps(csv::CsvReader& reader);
		int load_app_groups(csv::CsvReader& reader);
		int load_users(csv::CsvReader& reader);
		int load_user_groups(csv::CsvReader& reader);
		int load_urls(csv::CsvReader& reader);
		int load_url_groups(csv::CsvReader& reader);

		int load_addresses(const std::string& filename, f_interrupt_cb interrupt_cb);
		int load_address_groups(const std::string& filename, f_interrupt_cb interrupt_cb);
		int load_services(const std::string& filename, f_interrupt_cb interrupt_cb);
		int load_service_groups(const std::string& filename, f_interrupt_cb interrupt_cb);
		int load_apps(const std::string& filename, f_interrupt_cb interrupt_cb);
		int load_app_groups(const std::string& filename, f_interrupt_cb interrupt_cb);
		int load_users(const std::string& filename, f_interrupt_cb interrupt_cb);
		int load_user_groups(const std::string& filename, f_interrupt_cb interrupt_cb);
		int load_urls(const std::string& filename, f_interrupt_cb interrupt_cb);
		int load_url_groups(const std::string& filename, f_interrupt_cb interrupt_cb);

		AddressResolver::PoolMembers resolve_address_pool(const AddressPool* address_pool, std::list<std::string>& unresolved) const;
		ServiceResolver::PoolMembers resolve_service_pool(const ServicePool* service_pool, std::list<std::string>& unresolved) const;
		ApplicationResolver::PoolMembers resolve_application_pool(const ApplicationPool* application_pool, std::list<std::string>& unresolved) const;
		UserResolver::PoolMembers resolve_user_pool(const UserPool* user_pool, std::list<std::string>& unresolved) const;
		UrlResolver::PoolMembers resolve_url_pool(const UrlPool* url_pool, std::list<std::string>& unresolved) const;

		const AddressObject* get_address(const std::string& name) const;
		const AddressPool* get_address_pool(const std::string& name) const;
		const ServiceObject* get_service(const std::string& name) const;
		const ServicePool* get_service_pool(const std::string& name) const;
		const ApplicationObject* get_application(const std::string& name) const;
		const ApplicationPool* get_application_pool(const std::string& name) const;
		const UserObject* get_user(const std::string& name) const;
		const UserPool* get_user_pool(const std::string& name) const;
		const UrlObject* get_url(const std::string& name) const;
		const UrlPool* get_url_pool(const std::string& name) const;

		std::list<const AddressObject*> query_addresses(const std::string& name) const;
		std::list<const AddressPool*> query_address_pools(const std::string& name) const;
		std::list<const ServiceObject*> query_services(const std::string& name) const;
		std::list<const ServicePool*> query_service_pools(const std::string& name) const;
		std::list<const ApplicationObject*> query_application(const std::string& name) const;
		std::list<const ApplicationPool*> query_application_pools(const std::string& name) const;
		std::list<const UserObject*> query_user(const std::string& name) const;
		std::list<const UserPool*> query_user_pools(const std::string& name) const;
		std::list<const UrlObject*> query_url(const std::string& name) const;
		std::list<const UrlPool*> query_url_pools(const std::string& name) const;

	private:
		bool _initialized;
		IPAddressModel _ip_model;
		FqdnResolver _fqdn_resolver;

		AddressObjects _addresses;
		AddressPools _address_pools;
		AddressResolver _address_resolver;

		ServiceObjects _services;
		ServicePools _service_pools;
		ServiceResolver _service_resolver;

		ApplicationObjects _applications;
		ApplicationPools _application_pools;
		ApplicationResolver _application_resolver;

		UserObjects _users;
		UserPools _user_pools;
		UserResolver _user_resolver;

		UrlObjects _urls;
		UrlPools _url_pools;
		UrlResolver _url_resolver;
	};
}
