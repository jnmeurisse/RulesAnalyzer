/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#include "ostore/firewallfactory.h"

#include <cstdio>
#include <fstream>
#include <stdexcept>

#include "model/ipaddress.h"
#include "model/firewall.h"
#include "model/predicate.h"
#include "model/rule.h"
#include "model/serviceparser.h"
#include "model/url.h"
#include "ostore/genericresolver.h"
#include "ostore/policyreader.h"
#include "ostore/serviceobject.h"
#include "tools/strutil.h"


namespace fos {

	FirewallFactory::FirewallFactory(const fos::ObjectStore& object_store, const LoaderConfig& loader_config) :
		_object_store{ object_store },
		_loader_config{ loader_config },
		_logger{ Logger::get_logger() }
	{
	};


	LoaderStatus FirewallFactory::load(Firewall& fw, const std::string& csv_filename, f_interrupt_cb interrupt_cb)
	{
		std::ifstream csv_stream{ csv_filename, std::ifstream::in };
		if (!csv_stream.good())
			throw std::runtime_error("can't open file");

		fos::PolicyReader reader{ csv_stream, _loader_config.reader_config, interrupt_cb};

		return load_rules(fw, reader);
	}


	Firewall* FirewallFactory::clone(const Firewall& fw, const std::string& name) const
	{
		return new Firewall(fw, name);
	}


	LoaderStatus FirewallFactory::load_rules(Firewall& fw, PolicyReader& reader)
	{
		fos::RuleObject rule{};

		// Initialize the loader status
		LoaderStatus status;

		while (reader.next_row(rule)) {
			if (add_rule(fw, rule, status))
				status.loaded_count++;
			else
				status.error_count++;
		}

		if (_loader_config.add_implicit_deny_all)
			add_deny_all_rule(fw);

		status.disabled_count = status.loaded_count - fw.acl().size();

		return status;
	};


	SrcAddressGroupPtr FirewallFactory::build_src_address_group(Network& nw, const fos::RuleObject& rule, LoaderStatus& status)
	{
		SrcAddressGroupPtr src_address_group{ std::make_unique<SrcAddressGroup>("$root") };

		for (const std::string& src_address : rule.source_addresses) {
			if (is_ip_address(src_address, nw.config().ip_model, nw.config().strict_ip_parser)) {
				src_address_group->add_member(nw.register_src_address(src_address, src_address));
			}
			else {
				const AddressObject* address_object{ _object_store.get_address(src_address) };

				if (address_object && address_object->is_multi()) {
					src_address_group->add_member(
						nw.register_src_multi_address(address_object->name(), address_object->addresses())
					);
				}
				else if (address_object) {
					src_address_group->add_member(
						nw.register_src_address(address_object->name(), address_object->addresses()[0])
					);
				}
				else {
					const AddressPool* address_pool{ _object_store.get_address_pool(src_address) };

					if (address_pool) {
						for (const auto& member : _object_store.resolve_address_pool(address_pool, status.unresolved_addresses)) {
							if (member.is_pool) {
								nw.register_src_address_group(member.pool->name(), member.pool->members());
							}
							else if (member.object->is_multi()) {
								nw.register_src_multi_address(member.object->name(), member.object->addresses());
							}
							else {
								nw.register_src_address(member.object->name(), member.object->addresses()[0]);
							}
						}

						src_address_group->add_member(nw.get_src_address_group(src_address));
					}
					else {
						status.unresolved_addresses.push_back(src_address);
					}
				}
			}
		}

		return src_address_group;
	}


	DstAddressGroupPtr FirewallFactory::build_dst_address_group(Network& nw, const fos::RuleObject& rule, LoaderStatus& status)
	{
		DstAddressGroupPtr dst_address_group{ std::make_unique<DstAddressGroup>("$root") };

		for (const std::string& dst_address : rule.destination_addresses) {
			if (is_ip_address(dst_address, nw.config().ip_model, nw.config().strict_ip_parser)) {
				dst_address_group->add_member(nw.register_dst_address(dst_address, dst_address));
			}
			else {
				const AddressObject* address_object{ _object_store.get_address(dst_address) };

				if (address_object && address_object->is_multi()) {
					dst_address_group->add_member(
						nw.register_dst_multi_address(address_object->name(), address_object->addresses())
					);
				}
				else if (address_object) {
					dst_address_group->add_member(
						nw.register_dst_address(address_object->name(), address_object->addresses()[0])
					);
				}
				else {
					const AddressPool* address_pool{ _object_store.get_address_pool(dst_address) };

					if (address_pool) {
						for (const auto& member : _object_store.resolve_address_pool(address_pool, status.unresolved_addresses)) {
							if (member.is_pool) {
								nw.register_dst_address_group(member.pool->name(), member.pool->members());
							}
							else if (member.object->is_multi()) {
								nw.register_dst_multi_address(member.object->name(), member.object->addresses());
							}
							else {
								nw.register_dst_address(member.object->name(), member.object->addresses()[0]);
							}
						}

						dst_address_group->add_member(nw.get_dst_address_group(dst_address));
					}
					else {
						status.unresolved_addresses.push_back(dst_address);
					}
				}
			}
		}

		return dst_address_group;
	}


	ServiceGroupPtr FirewallFactory::build_service_group(Network& nw, const fos::RuleObject& rule, LoaderStatus& status)
	{
		ServiceGroupPtr service_group{ std::make_unique<ServiceGroup>("$root") };

		if (rule.default_app_services()) {
			service_group->add_member(nw.register_service("any", "any"));
		}
		else {
			for (const std::string& service : rule.services) {
				if (is_protocol_port(service)) {
					service_group->add_member(nw.register_service(service, service));
				}
				else {
					const ServiceObject* service_object{ _object_store.get_service(service) };

					if (service_object) {
						if (service_object->is_multi()) {
							service_group->add_member(nw.register_multi_service(service_object->name(), service_object->services()));
						}
						else {
							service_group->add_member(nw.register_service(service_object->name(), service_object->services()[0]));
						}
					}
					else {
						const ServicePool* service_pool{ _object_store.get_service_pool(service) };

						if (service_pool) {
							for (const auto& member : _object_store.resolve_service_pool(service_pool, status.unresolved_services)) {
								if (member.is_pool) {
									nw.register_service_group(member.pool->name(), member.pool->members());
								}
								else if (member.object->is_multi()) {
									nw.register_multi_service(member.object->name(), member.object->services());
								}
								else {
									nw.register_service(member.object->name(), member.object->services()[0]);
								}
							}

							service_group->add_member(nw.get_service_group(service));
						}
						else {
							status.unresolved_services.push_back(service);
						}
					}
				}
			}
		}

		return service_group;
	}


	ApplicationGroupPtr FirewallFactory::build_application_group(Network& nw, const fos::RuleObject& rule, LoaderStatus& status) const
	{
		ApplicationGroupPtr application_group{ std::make_unique<ApplicationGroup>("$root") };

		for (const std::string& application : rule.applications) {
			const ApplicationObject* application_object{ _object_store.get_application(application) };

			if (application_object) {
				application_group->add_member(
					nw.register_application(
						application_object->name(),
						resolve_app_services(application_object->services()),
						rule.default_app_services()
					)
				);
			}
			else {
				const ApplicationPool* application_pool{ _object_store.get_application_pool(application) };

				if (application_pool) {
					for (const auto& member : _object_store.resolve_application_pool(application_pool, status.unresolved_applications)) {
						if (member.is_pool) {
							nw.register_application_group(
								member.pool->name(),
								member.pool->members(),
								rule.default_app_services()
							);
						}
						else {
							nw.register_application(
								member.object->name(),
								resolve_app_services(member.object->services()),
								rule.default_app_services()
							);
						}
					}

					application_group->add_member(
						nw.get_application_group(application, rule.default_app_services())
					);
				}
				else {
					status.unresolved_applications.push_back(application);
				}
			}
		}

		return application_group;
	}


	SrcZoneGroupPtr FirewallFactory::build_src_zone_group(Network& nw, const fos::RuleObject& rule, LoaderStatus& status) const
	{
		SrcZoneGroupPtr src_zone_group{ std::make_unique<SrcZoneGroup>("$root") };

		for (const std::string& zone : rule.source_zones) {
			src_zone_group->add_member(nw.register_src_zone(zone));
		}

		return src_zone_group;
	}


	DstZoneGroupPtr FirewallFactory::build_dst_zone_group(Network& nw, const fos::RuleObject& rule, LoaderStatus& status) const
	{
		DstZoneGroupPtr dst_zone_group{ std::make_unique<DstZoneGroup>("$root") };

		for (const std::string& zone : rule.destination_zones) {
			dst_zone_group->add_member(nw.register_dst_zone(zone));
		}

		return dst_zone_group;
	}


	UserGroupPtr FirewallFactory::build_user_group(Network& nw, const fos::RuleObject& rule, LoaderStatus& status) const
	{
		UserGroupPtr user_group{ std::make_unique<UserGroup>("$root") };

		for (const std::string& user : rule.users) {
			const UserObject* user_object{ _object_store.get_user(user) };

			if (user_object) {
				user_group->add_member(nw.register_user(user_object->name()));
			}
			else {
				const UserPool* user_pool{ _object_store.get_user_pool(user) };

				if (user_pool) {
					for (const auto& member : _object_store.resolve_user_pool(user_pool, status.unresolved_users)) {
						if (member.is_pool) {
							nw.register_user_group(member.pool->name(), member.pool->members());
						}
						else {
							nw.register_user(member.object->name());
						}
					}

					user_group->add_member(nw.get_user_group(user));
				}
				else {
					status.unresolved_users.push_back(user);
				}
			}
		}

		return user_group;
	}


	UrlGroupPtr FirewallFactory::build_url_group(Network& nw, const fos::RuleObject& rule, LoaderStatus& status) const
	{
		UrlGroupPtr url_group{ std::make_unique<UrlGroup>("$root") };

		for (const std::string& url : rule.urls) {
			if (is_url(url)) {
				url_group->add_member(nw.register_url(url));
			}
			else {
				const UrlObject* url_object{ _object_store.get_url(url) };

				if (url_object) {
					url_group->add_member(nw.register_url(url_object->name()));
				}
				else {
					const UrlPool* url_pool{ _object_store.get_url_pool(url) };

					if (url_pool) {
						for (const auto& member : _object_store.resolve_url_pool(url_pool, status.unresolved_urls)) {
							if (member.is_pool) {
								nw.register_url_group(member.pool->name(), member.pool->members());
							}
							else {
								nw.register_url(member.object->name());
							}
						}

						url_group->add_member(nw.get_url_group(url));
					}
					else {
						status.unresolved_urls.push_back(url);
					}
				}
			}
		}

		return url_group;
	}



	bool FirewallFactory::add_rule(Firewall& fw, const fos::RuleObject& rule, LoaderStatus& status)
	{
		RuleAction rule_action;
		RuleStatus rule_status;
		bool valid{ true };

		if (fw.get_rule(rule.id)) {
			_logger->error("row %d : rule id %d is duplicated", rule.row_number, rule.id);
			valid = false;
		}

		if (rat::iequal(rule.action, "allow"))
			rule_action = RuleAction::ALLOW;
		else if (rat::iequal(rule.action, "accept"))
			rule_action = RuleAction::ALLOW;
		else if (rat::iequal(rule.action, "drop"))
			rule_action = RuleAction::DENY;
		else if (rat::iequal(rule.action, "deny"))
			rule_action = RuleAction::DENY;
		else {
			_logger->error("row %d (rule id %d) : '%s' is an invalid action",
				rule.row_number,
				rule.id,
				rule.action.c_str()
			);
			return false;
		}

		if (rat::iequal(rule.status, "enable") || rat::iequal(rule.status, "enabled"))
			rule_status = RuleStatus::ENABLED;
		else if (rat::iequal(rule.status, "disable") || rat::iequal(rule.status, "disabled"))
			rule_status = RuleStatus::DISABLED;
		else {
			_logger->error("row %d (rule id %d) : '%s' is an invalid status",
				rule.row_number,
				rule.id,
				rule.status.c_str()
			);
			return false;
		}

		Network& network{ fw.network() };

		SrcAddressGroupPtr src_address_group{ build_src_address_group(network, rule, status) };
		if (src_address_group->empty()) {
			status.empty_sources.push_back(rule.row_number);
			valid = false;
		}

		DstAddressGroupPtr dst_address_group{ build_dst_address_group(network, rule, status) };
		if (dst_address_group->empty()) {
			status.empty_destinations.push_back(rule.row_number);
			valid = false;
		}

		ApplicationGroupPtr application_group{ build_application_group(network, rule, status) };
		if (application_group->empty()) {
			status.empty_applications.push_back(rule.row_number);
			valid = false;
		}

		ServiceGroupPtr service_group;
		if (rule.default_app_services()) {
			service_group = application_group->default_services();
		}
		else {
			service_group = build_service_group(network, rule, status);
		}
		if (service_group->empty()) {
			status.empty_services.push_back(rule.row_number);
			valid = false;
		}

		SrcZoneGroupPtr src_zone_list{ build_src_zone_group(network, rule, status) };
		if (src_zone_list->empty()) {
			status.empty_src_zones.push_back(rule.row_number);
			valid = false;
		}

		DstZoneGroupPtr dst_zone_list{ build_dst_zone_group(network, rule, status) };
		if (dst_zone_list->empty()) {
			status.empty_dst_zones.push_back(rule.row_number);
			valid = false;
		}

		UserGroupPtr user_group{ build_user_group(network, rule, status) };
		if (user_group->empty()) {
			status.empty_users.push_back(rule.row_number);
			valid = false;
		}

		UrlGroupPtr url_group { build_url_group(network, rule, status) };
		if (url_group->empty()) {
			status.empty_urls.push_back(rule.row_number);
			valid = false;
		}

		if (valid) {
			// Allocate a predicate.  All lists are containing pointers
			// into the object caches.  The lifetime of the cache must
			// be greater than the predicate lifetime.
			PredicatePtr predicate = std::make_unique<Predicate>(
				Sources{ src_zone_list.release(), src_address_group.release(), rule.negate_source_addresses },
				Destinations{ dst_zone_list.release(), dst_address_group.release(), rule.negate_destination_addresses },
				service_group.release(),
				application_group.release(),
				user_group.release(),
				url_group.release()
			);

			// Add an new rule to the firewall Access Control List.
			fw.add_rule(
				new Rule(
					fw,
					rule.name,
					rule.id,
					rule_status,
					rule_action,
					predicate.release()
				)
			);
		}

		return valid;
	}


	std::vector<std::string> FirewallFactory::resolve_app_services(const std::vector<std::string>& application_services) const
	{
		std::vector<std::string> resolved_application_services;

		for (const std::string& application_service : application_services) {
			if (is_protocol_port(application_service)) {
				resolved_application_services.push_back(application_service);
			}
			else {
				const ServiceObject* service_object = _object_store.get_service(application_service);
				if (service_object) {
					for (const std::string& service : service_object->services())
						resolved_application_services.push_back(service);
				}
				else {
					// This should never occur.
					// The ApplicationObjects.make_object method has checked that the service exist.
					printf("*** ERROR *** : application service '%s' not found in resolve_app_services\n", application_service.c_str());
				}
			}
		}

		return resolved_application_services;
	}


	bool FirewallFactory::add_deny_all_rule(fwm::Firewall& fw) const
	{
		// Add a new deny rule to the firewall Access Control List.
		fw.add_rule(
			new Rule(
				fw,
				"",
				0,
				RuleStatus::ENABLED,
				RuleAction::DENY,
				Predicate::any(fw.network().config().ip_model)
			)
		);

		return true;
	}

}
