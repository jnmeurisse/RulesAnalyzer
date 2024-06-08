/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#include "cache.h"
#include "rulelist.h"
#include "zone.h"

#include <algorithm>
#include <set>


namespace fwm {

	RuleList RuleList::filter(std::function<bool(const Rule&)> predicate) const
	{
		RuleList rules;

		for (const Rule* rule : *this) {
			if (predicate(*rule))
				rules.push_back(rule);
		}

		return rules;
	}


	RuleList RuleList::filter_if(std::function<bool(const Rule&)> predicate, bool condition) const
	{
		if (condition)
			return filter(predicate);
		else
			return RuleList(*this);
	}


	RuleList RuleList::filter(RuleAction action_filter) const
	{
		auto select_function = [action_filter](const Rule& rule) -> bool {
			return rule.action() == action_filter;
		};

		return filter(select_function);
	}


	RuleList RuleList::filter(RuleStatus status_filter) const
	{
		auto select_function = [status_filter](const Rule& rule) -> bool {
			return rule.status() == status_filter;
		};

		return filter(select_function);
	}


	RuleList RuleList::filter(const SrcZone& src_zone, const DstZone& dst_zone) const
	{
		auto select_function = [src_zone, dst_zone](const Rule& rule) -> bool {
				return 
					src_zone.is_subset(rule.predicate().src_zones()) &&
					dst_zone.is_subset(rule.predicate().dst_zones());
		};

		return RuleList::filter(select_function);
	}


	RuleList RuleList::filter_if(const ZonePair& zones) const
	{
		if (zones.src_zone && zones.dst_zone)
			return filter(*zones.src_zone, *zones.dst_zone);
		else
			return RuleList(*this);
	}


	RuleList RuleList::filter(const SrcAddress* address) const
	{
		auto select_function = [address](const Rule& rule) -> bool {
			return rule.predicate().src_addresses().contains(address);
		};

		return RuleList::filter(select_function);
	}


	RuleList RuleList::filter(const DstAddress* address) const
	{
		auto select_function = [address](const Rule& rule) -> bool {
			return rule.predicate().dst_addresses().contains(address);
		};

		return RuleList::filter(select_function);
	}


	RuleList RuleList::filter(const Service* service) const
	{
		auto select_function = [service](const Rule& rule) -> bool {
			return rule.predicate().services().contains(service);
		};

		return RuleList::filter(select_function);
	}


	RuleList RuleList::filter(const Application* application) const
	{
		auto select_function = [application](const Rule& rule) -> bool {
			return rule.predicate().applications().contains(application);
		};

		return RuleList::filter(select_function);
	}


	RuleList RuleList::filter(const User* user) const
	{
		auto select_function = [user](const Rule& rule) -> bool {
			return rule.predicate().users().contains(user);
		};

		return RuleList::filter(select_function);
	}


	RuleList RuleList::filter(const ServiceGroup& services) const
	{
		auto select_func = [&services](const Rule& rule) -> bool {
				return services.is_subset(rule.predicate().services());
		};

		return filter(select_func);
	}


	RuleList RuleList::filter(const ApplicationGroup& applications, const ServiceGroup& services) const
	{
		RuleList rules;

		if (services.empty()) {
			auto select_func = [&applications](const Rule& rule) -> bool {
				return applications.is_subset(rule.predicate().applications());
			};

			rules = filter(select_func);
		}
		else {
			auto select_func = [&applications, &services](const Rule& rule) -> bool {
				const Predicate& predicate{ rule.predicate() };
				bool selected;

				if (rule.is_default_app_svc()) {
					std::list<const Application*> application_list{ applications.items() };

					// Each (application, services) pair must be checked independently against each
					// default application services.  The rule is selected only if all pairs are
					// subset of applications defined in the rule.  A copy of the service
					// group is mandatory since the created application takes the ownership
					// of the services.
					selected = std::all_of(
						application_list.begin(),
						application_list.end(),
						[&predicate, &services](const Application* application) -> bool {
							// Copy of the services.
							ServiceGroupPtr svc_group{ new ServiceGroup("") };
							for (const Service* service : services.items())
								svc_group->add_member(service);

							// Create a copy of the application that combines the given services.
							std::unique_ptr<const Application> app{ Application::create(
								application->name(),
								application->id(),
								svc_group,
								application->options(),
								true)
							};

							// Check if this (application, service) pair is a subset of the
							// applications defined in the give rule.
							return app->is_subset(predicate.applications());
						}
					);
				}
				else {
					selected = applications.is_subset(predicate.applications()) &&
						services.is_subset(predicate.services());
				}

				return selected;
			};

			rules = filter(select_func);
		}

		return rules;
	}


	RuleList RuleList::filter_before(const Rule* sentinel, std::function<bool(const Rule&)> predicate) const
	{
		RuleList rules;

		for (const Rule* rule : *this) {
			if (rule == sentinel)
				return rules;
			else if (predicate(*rule))
				rules.push_back(rule);
		}

		return rules;
	}


	ZoneNames RuleList::zone_names() const
	{
		ZoneNames names;

		for (const Rule* rule : *this) {
			for (const SrcZone* zone : rule->predicate().src_zones().items()) {
				names.push_back(zone->name());
			}

			for (const DstZone* zone : rule->predicate().dst_zones().items()) {
				names.push_back(zone->name());
			}
		}

		// Keep unique zone names
		return names.unique();
	}


	ZoneNames RuleList::src_zone_names() const
	{
		ZoneNames names;

		for (const Rule* rule : *this) {
			for (const SrcZone* zone : rule->predicate().src_zones().items()) {
				names.push_back(zone->name());
			}
		}

		// Keep unique zone names
		return names.unique();
	}


	ZoneNames RuleList::dst_zone_names() const
	{
		ZoneNames names;

		for (const Rule* rule : *this) {
			for (const DstZone* zone : rule->predicate().dst_zones().items()) {
				names.push_back(zone->name());
			}
		}

		// Keep unique zone names
		return names.unique();
	}


	SrcAddressGroupPtr RuleList::source_addresses() const
	{
		SrcAddressGroupPtr addresses{ std::make_unique<SrcAddressGroup>("") };

		for (const Rule* rule : *this) {
			for (const SrcAddress* address: rule->predicate().src_addresses().items()) {
				addresses->add_member(address);
			}
		}

		return addresses;
	}


	DstAddressGroupPtr RuleList::destination_addresses() const
	{
		DstAddressGroupPtr addresses{ std::make_unique<DstAddressGroup>("") };

		for (const Rule* rule : *this) {
			for (const DstAddress* address : rule->predicate().dst_addresses().items()) {
				addresses->add_member(address);
			}
		}

		return addresses;
	}


	AddressGroupPtr RuleList::addresses() const
	{
		AddressGroupPtr addresses{ std::make_unique<AddressGroup>("") };
		std::set<std::string> names;

		for (const SrcAddress* src_address : source_addresses()->items()) {
			if (names.insert(src_address->name()).second) {
				addresses->add_member(src_address);
			}
		}

		for (const DstAddress* dst_address : destination_addresses()->items()) {
			if (names.insert(dst_address->name()).second) {
				addresses->add_member(dst_address);
			}
		}

		return addresses;
	}


	ServiceGroupPtr RuleList::services() const
	{
		ServiceGroupPtr services{ std::make_unique<ServiceGroup>("") };

		for (const Rule* rule : *this) {
			for (const Service* service : rule->predicate().services().items()) {
				services->add_member(service);
			}
		}

		return services;
	}


	ApplicationGroupPtr RuleList::applications() const
	{
		ApplicationGroupPtr applications{ std::make_unique<ApplicationGroup>("") };

		for (const Rule* rule : *this) {
			for (const Application* application : rule->predicate().applications().items()) {
				applications->add_member(application);
			}
		}

		return applications;
	}


	UserGroupPtr RuleList::users() const
	{
		UserGroupPtr users{ std::make_unique<UserGroup>("") };

		for (const Rule* rule : *this) {
			for (const User* user : rule->predicate().users().items()) {
				users->add_member(user);
			}
		}

		return users;
	}


	bool RuleList::is_last(const Rule* rule) const
	{
		return rule == back();
	}


	std::vector<int> RuleList::id_list() const
	{
		std::vector<int> id_list;
		for (const Rule* rule : *this)
			id_list.push_back(rule->id());

		return id_list;
	}


	bool RuleList::have_names() const
	{
		return std::any_of(
					cbegin(),
					cend(),
					[](const Rule* rule) { return !rule->name().empty(); }
		);
	}

	bool RuleList::have_negate() const
	{
		return std::any_of(
			cbegin(),
			cend(),
			[](const Rule* rule) {
				const Predicate& predicate = rule->predicate();
				return predicate.negate_src_addresses() || predicate.negate_dst_addresses();
			}
		);
	}


	RuleList::Counters RuleList::get_counters() const
	{
		Counters counters{ 0, 0 };

		for (const Rule* rule : *this) {
			if (rule->action() == RuleAction::ALLOW)
				counters.allowed++;
			else
				counters.denied++;
		}

		return counters;
	}


	Table RuleList::output(const RuleOutputOptions& options) const
	{
		std::vector<std::string> columns;
		columns.push_back("id");
		if (options.contains(RuleOutputOption::RULE_NAME))
			columns.push_back("name");
		columns.push_back("action");
		columns.push_back("src.zone");
		columns.push_back("dst.zone");
		if (options.contains(RuleOutputOption::NEGATE_ADDRESS))
			columns.push_back("src.negate");
		if (options.contains(RuleOutputOption::ADDRESS_NAME))
			columns.push_back("src.addr");
		columns.push_back("src.ip");
		if (options.contains(RuleOutputOption::NEGATE_ADDRESS))
			columns.push_back("dst.negate");
		if (options.contains(RuleOutputOption::ADDRESS_NAME))
			columns.push_back("dst.addr");
		columns.push_back("dst.ip");
		if (options.contains(RuleOutputOption::SERVICE_NAME))
			columns.push_back("svc name");
		columns.push_back("svc");
		if (options.contains(RuleOutputOption::APPLICATION_NAME))
			columns.push_back("app");
		if (options.contains(RuleOutputOption::USER_NAME))
			columns.push_back("user");

		Table table{ columns };

		for (const Rule* rule : *this) {
			rule->output(table.add_row(), options);
		}

		return table;
	}


	std::string RuleList::Counters::to_string() const
	{
		return std::to_string(allowed) + "/" + std::to_string(denied);
	}

}
