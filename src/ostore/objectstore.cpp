/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#include "ostore/objectstore.h"

#include <cstdio>
#include <cassert>
#include <fstream>
#include <string>
#include <stdexcept>
#include <vector>

#include "tools/strutil.h"


namespace fos {

	ObjectStore::ObjectStore(const OstoreConfig& config) :
		_initialized{ false },
		_ip_model{ config.model_config.ip_model },
		_fqdn_resolver{ config.fqdn_resolver_config },
		_addresses{ config.loader_config.reader_config, config.model_config, _fqdn_resolver },
		_address_pools{ config.loader_config.reader_config },
		_address_resolver{ _addresses, _address_pools },
		_services{ config.loader_config.reader_config },
		_service_pools{ config.loader_config.reader_config },
		_service_resolver{ _services, _service_pools },
		_applications{ config.loader_config.reader_config, _services },
		_application_pools{ config.loader_config.reader_config },
		_application_resolver{ _applications, _application_pools},
		_users{ config.loader_config.reader_config },
		_user_pools{ config.loader_config.reader_config },
		_user_resolver{ _users, _user_pools },
		_urls{ config.loader_config.reader_config },
		_url_pools{ config.loader_config.reader_config },
		_url_resolver{ _urls, _url_pools }
	{
	}


	void ObjectStore::initialize()
	{
		clear();
		_fqdn_resolver.initialize();
		_initialized = true;
	}


	void ObjectStore::terminate()
	{
		clear();
		_fqdn_resolver.terminate();
		_initialized = false;
	}


	void ObjectStore::clear()
	{
		_addresses.clear();
		_address_pools.clear();
		_services.clear();
		_service_pools.clear();
		_applications.clear();
		_application_pools.clear();
		_users.clear();
		_user_pools.clear();
		_urls.clear();
		_url_pools.clear();

		if (_ip_model == IPAddressModel::IP64Model) {
			_addresses.add(new IpmaskAddressObject("any4", { "any4" }));
			_addresses.add(new IpmaskAddressObject("any6", { "any6" }));
			_address_pools.add(new AddressPool("any", { "any4", "any6" }));
		}
		else
			_addresses.add(new IpmaskAddressObject("any", { "any" }));

		_services.add(new ServiceObject("any", "any"));
		_services.add(new ServiceObject("tcp", "tcp"));
		_services.add(new ServiceObject("udp", "udp"));
		_services.add(new ServiceObject("icmp", "icmp"));

		_applications.add(new ApplicationObject("any", "any"));
		_applications.add(new ApplicationObject("udp", "udp"));
		_applications.add(new ApplicationObject("tcp", "tcp"));
		_applications.add(new ApplicationObject("icmp", "icmp"));

		_users.add(new UserObject("any"));

		_urls.add(new UrlObject("any"));
	}


	Table ObjectStore::info()
	{
		Table table{ { "store", "objects" } };
		Row *row;

		row = &table.add_row();
		row->cell(0).append("addresses");
		row->cell(1).append(_addresses.size());
		row = &table.add_row();
		row->cell(0).append("address groups");
		row->cell(1).append(_address_pools.size());
		row = &table.add_row();
		row->cell(0).append("services");
		row->cell(1).append(_services.size());
		row = &table.add_row();
		row->cell(0).append("service groups");
		row->cell(1).append(_service_pools.size());
		row = &table.add_row();
		row->cell(0).append("applications");
		row->cell(1).append(_applications.size());
		row = &table.add_row();
		row->cell(0).append("application groups");
		row->cell(1).append(_application_pools.size());
		row = &table.add_row();
		row->cell(0).append("users");
		row->cell(1).append(_users.size());
		row = &table.add_row();
		row->cell(0).append("user groups");
		row->cell(1).append(_user_pools.size());
		row = &table.add_row();
		row->cell(0).append("urls");
		row->cell(1).append(_urls.size());
		row = &table.add_row();
		row->cell(0).append("url groups");
		row->cell(1).append(_url_pools.size());

		return table;
	}


	int ObjectStore::load_addresses(csv::CsvReader& reader)
	{
		assert(_initialized);
		return _addresses.load(reader);
	}


	int ObjectStore::load_address_pools(csv::CsvReader& reader)
	{
		assert(_initialized);
		return _address_pools.load(reader);
	}


	int ObjectStore::load_services(csv::CsvReader& reader)
	{
		assert(_initialized);
		return _services.load(reader);
	}


	int ObjectStore::load_service_groups(csv::CsvReader& reader)
	{
		assert(_initialized);
		return _service_pools.load(reader);
	}


	int ObjectStore::load_apps(csv::CsvReader& reader)
	{
		assert(_initialized);
		return _applications.load(reader);
	}


	int ObjectStore::load_app_groups(csv::CsvReader& reader)
	{
		assert(_initialized);
		return _application_pools.load(reader);
	}


	int ObjectStore::load_users(csv::CsvReader & reader)
	{
		assert(_initialized);
		return _users.load(reader);
	}


	int ObjectStore::load_user_groups(csv::CsvReader & reader)
	{
		assert(_initialized);
		return _user_pools.load(reader);
	}


	int ObjectStore::load_urls(csv::CsvReader& reader)
	{
		assert(_initialized);
		return _urls.load(reader);
	}


	int ObjectStore::load_url_groups(csv::CsvReader& reader)
	{
		assert(_initialized);
		return _url_pools.load(reader);
	}



	int ObjectStore::load_addresses(const std::string& filename, f_interrupt_cb interrupt_cb)
	{
		std::ifstream csv_file{ filename, std::ifstream::in };
		if (!csv_file.good()) {
			throw std::runtime_error("can't open file");
		}

		csv::CsvReader reader{ csv_file, AddressObjects::parameters(), interrupt_cb };
		return load_addresses(reader);
	}


	int ObjectStore::load_address_groups(const std::string& filename, f_interrupt_cb interrupt_cb)
	{
		std::ifstream csv_file{ filename, std::ifstream::in };
		if (!csv_file.good()) {
			throw std::runtime_error("can't open file ");
		}

		csv::CsvReader reader{ csv_file, AddressPools::parameters(), interrupt_cb };
		return load_address_pools(reader);
	}


	int ObjectStore::load_services(const std::string& filename, f_interrupt_cb interrupt_cb)
	{
		std::ifstream csv_file{ filename, std::ifstream::in };
		if (!csv_file.good()) {
			throw std::runtime_error("can't open file");
		}

		csv::CsvReader reader{ csv_file, ServiceObjects::parameters(), interrupt_cb };
		return load_services(reader);
	}


	int ObjectStore::load_service_groups(const std::string& filename, f_interrupt_cb interrupt_cb)
	{
		std::ifstream csv_file{ filename, std::ifstream::in };
		if (!csv_file.good()) {
			throw std::runtime_error("can't open file");
		}

		csv::CsvReader reader{ csv_file, ServicePools::parameters(), interrupt_cb };
		return load_service_groups(reader);
	}


	int ObjectStore::load_apps(const std::string& filename, f_interrupt_cb interrupt_cb)
	{
		std::ifstream csv_file{ filename, std::ifstream::in };
		if (!csv_file.good()) {
			throw std::runtime_error("can't open file");
		}

		csv::CsvReader reader{ csv_file, ApplicationObjects::parameters(), interrupt_cb };
		return load_apps(reader);
	}


	int ObjectStore::load_app_groups(const std::string& filename, f_interrupt_cb interrupt_cb)
	{
		std::ifstream csv_file{ filename, std::ifstream::in };
		if (!csv_file.good()) {
			throw std::runtime_error("can't open file");
		}

		csv::CsvReader reader{ csv_file, ApplicationPools::parameters(), interrupt_cb };
		return load_app_groups(reader);
	}


	int ObjectStore::load_users(const std::string & filename, f_interrupt_cb interrupt_cb)
	{
		std::ifstream csv_file{ filename, std::ifstream::in };
		if (!csv_file.good()) {
			throw std::runtime_error("can't open file");
		}

		csv::CsvReader reader{ csv_file, UserObjects::parameters(), interrupt_cb };
		return load_users(reader);
	}


	int ObjectStore::load_user_groups(const std::string & filename, f_interrupt_cb interrupt_cb)
	{
		std::ifstream csv_file{ filename, std::ifstream::in };
		if (!csv_file.good()) {
			throw std::runtime_error("can't open file");
		}

		csv::CsvReader reader{ csv_file, UserPools::parameters(), interrupt_cb };
		return load_user_groups(reader);
	}


	int ObjectStore::load_urls(const std::string& filename, f_interrupt_cb interrupt_cb)
	{
		std::ifstream csv_file{ filename, std::ifstream::in };
		if (!csv_file.good()) {
			throw std::runtime_error("can't open file");
		}

		csv::CsvReader reader{ csv_file, UrlObjects::parameters(), interrupt_cb };
		return load_urls(reader);
	}


	int ObjectStore::load_url_groups(const std::string& filename, f_interrupt_cb interrupt_cb)
	{
		std::ifstream csv_file{ filename, std::ifstream::in };
		if (!csv_file.good()) {
			throw std::runtime_error("can't open file");
		}

		csv::CsvReader reader{ csv_file, UrlPools::parameters(), interrupt_cb };
		return load_url_groups(reader);
	}


	AddressResolver::PoolMembers ObjectStore::resolve_address_pool(const AddressPool* address_pool, std::list<std::string>& unresolved) const
	{
		return _address_resolver.resolve(address_pool, unresolved);
	}


	ServiceResolver::PoolMembers ObjectStore::resolve_service_pool(const ServicePool* service_pool, std::list<std::string>& unresolved) const
	{
		return _service_resolver.resolve(service_pool, unresolved);
	}


	ApplicationResolver::PoolMembers ObjectStore::resolve_application_pool(const ApplicationPool* application_pool, std::list<std::string>& unresolved) const
	{
		return _application_resolver.resolve(application_pool, unresolved);
	}


	UserResolver::PoolMembers ObjectStore::resolve_user_pool(const UserPool* user_pool, std::list<std::string>& unresolved) const
	{
		return _user_resolver.resolve(user_pool, unresolved);
	}


	UrlResolver::PoolMembers ObjectStore::resolve_url_pool(const UrlPool* url_pool, std::list<std::string>& unresolved) const
	{
		return _url_resolver.resolve(url_pool, unresolved);
	}


	const AddressObject* ObjectStore::get_address(const std::string& name) const
	{
		return _addresses.get(name);
	}


	const AddressPool* ObjectStore::get_address_pool(const std::string& name) const
	{
		return _address_pools.get(name);
	}


	const ServiceObject* ObjectStore::get_service(const std::string& name) const
	{
		return _services.get(name);
	}


	const ServicePool* ObjectStore::get_service_pool(const std::string& name) const
	{
		return _service_pools.get(name);
	}


	const ApplicationObject* ObjectStore::get_application(const std::string& name) const
	{
		return _applications.get(name);
	}


	const ApplicationPool* ObjectStore::get_application_pool(const std::string& name) const
	{
		return _application_pools.get(name);
	}


	const UserObject* ObjectStore::get_user(const std::string & name) const
	{
		return _users.get(name);
	}


	const UserPool* ObjectStore::get_user_pool(const std::string& name) const
	{
		return _user_pools.get(name);
	}


	const UrlObject* ObjectStore::get_url(const std::string& name) const
	{
		return _urls.get(name);
	}


	const UrlPool* ObjectStore::get_url_pool(const std::string& name) const
	{
		return _url_pools.get(name);
	}


	std::list<const AddressObject*> ObjectStore::query_addresses(const std::string& name) const
	{
		return _addresses.query(name);
	}


	std::list<const AddressPool*> ObjectStore::query_address_pools(const std::string& name) const
	{
		return _address_pools.query(name);
	}


	std::list<const ServiceObject*> ObjectStore::query_services(const std::string& name) const
	{
		return _services.query(name);
	}


	std::list<const ServicePool*> ObjectStore::query_service_pools(const std::string& name) const
	{
		return _service_pools.query(name);
	}


	std::list<const ApplicationObject*> ObjectStore::query_application(const std::string& name) const
	{
		return _applications.query(name);
	}


	std::list<const ApplicationPool*> ObjectStore::query_application_pools(const std::string& name) const
	{
		return _application_pools.query(name);
	}


	std::list<const UserObject*> ObjectStore::query_user(const std::string& name) const
	{
		return _users.query(name);
	}


	std::list<const UserPool*> ObjectStore::query_user_pools(const std::string & name) const
	{
		return _user_pools.query(name);
	}


	std::list<const UrlObject*> ObjectStore::query_url(const std::string& name) const
	{
		return _urls.query(name);
	}


	std::list<const UrlPool*> ObjectStore::query_url_pools(const std::string& name) const
	{
		return _url_pools.query(name);
	}

}
