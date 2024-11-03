/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#include "model/rulelist.h"

#include <algorithm>
#include <set>

#include "model/address.h"
#include "cache.h"
#include "zone.h"


namespace fwm {

	RuleList::RuleList(size_t default_size) :
		RuleList()
	{
		_rules.reserve(default_size);
	}


	void RuleList::push_back(const Rule* rule)
	{
		 _rules.push_back(rule);
	}


	RuleList::const_iterator RuleList::begin() const
	{
		return _rules.begin();
	}


	RuleList::const_iterator RuleList::end() const
	{
		return _rules.end();
	}


	const Rule* RuleList::front() const
	{
		return _rules.front();
	}


	const Rule* RuleList::back() const
	{
		return _rules.back();
	}


	size_t RuleList::size() const
	{
		return _rules.size();
	}


	void RuleList::clear()
	{
		_rules.clear();
	}


	RuleList RuleList::filter(std::function<bool(const Rule&)> predicate) const
	{
		RuleList rules(this->size());

		for (const Rule* rule : _rules) {
			if (predicate(*rule))
				rules.push_back(rule);
		}

		return rules;
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
		auto select_function = [&src_zone, &dst_zone](const Rule& rule) -> bool {
				return
					src_zone.is_subset(rule.predicate().src_zones()) &&
					dst_zone.is_subset(rule.predicate().dst_zones());
		};

		return RuleList::filter(select_function);
	}


	RuleList RuleList::filter(const ZonePair& zones) const
	{
		return filter(zones.src_zone, zones.dst_zone);
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


	RuleList RuleList::filter(const Url* url) const
	{
		auto select_function = [url](const Rule& rule) -> bool {
			return rule.predicate().urls().contains(url);
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
					auto application_list{ applications.items() };

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
							std::unique_ptr<const Application> app{ new Application(
								*application,
								svc_group.release())
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
		RuleList rules(this->size());

		for (const Rule* rule : _rules) {
			if (rule == sentinel)
				return rules;
			else if (predicate(*rule))
				rules.push_back(rule);
		}

		return rules;
	}


	ZoneListPtr RuleList::all_zones(const std::set<const Zone*> excluded_set) const
	{
		ZoneListPtr zones = std::make_unique<ZoneList>();

		// Get all zones from all rules.
		for (const Rule* rule : _rules) {
			const Predicate& predicate = rule->predicate();

			for (const SrcZone* src_zone : predicate.src_zones().items()) {
				if (excluded_set.find(src_zone) == excluded_set.end())
					zones->push_back(src_zone);
			}

			for (const DstZone* dst_zone : predicate.dst_zones().items()) {
				if (excluded_set.find(dst_zone) == excluded_set.end())
					zones->push_back(dst_zone);
			}
		}

		return zones;
	}


	SrcZoneListPtr RuleList::all_src_zones(const std::set<const SrcZone*>& excluded_set) const
	{
		SrcZoneListPtr zones = std::make_unique<SrcZoneList>();

		// Get all zones from all rules.
		for (const Rule* rule : _rules) {
			const Predicate& predicate = rule->predicate();

			for (const SrcZone* src_zone : predicate.src_zones().items()) {
				if (excluded_set.find(src_zone) == excluded_set.end())
					zones->push_back(src_zone);
			}
		}

		return zones;
	}


	DstZoneListPtr RuleList::all_dst_zones(const std::set<const DstZone*>& excluded_set) const
	{
		DstZoneListPtr zones = std::make_unique<DstZoneList>();

		// Get all zones from all rules.
		for (const Rule* rule : _rules) {
			const Predicate& predicate = rule->predicate();

			for (const DstZone* dst_zone : predicate.dst_zones().items()) {
				if (excluded_set.find(dst_zone) == excluded_set.end())
					zones->push_back(dst_zone);
			}
		}

		// Sort by names
		zones->sort();

		return zones;
	}


	AddressListPtr RuleList::all_addresses(const std::set<const IpAddress*>& excluded_set) const
	{
		AddressListPtr addresses = std::make_unique<AddressList>();

		// Get all addresses from all rules.
		for (const Rule* rule : _rules) {
			const Predicate& predicate = rule->predicate();

			for (const SrcAddress* src_address : predicate.src_addresses().items()) {
				if (excluded_set.find(src_address) == excluded_set.end())
					addresses->push_back(src_address);
			}

			for (const DstAddress* dst_address : predicate.dst_addresses().items()) {
				if (excluded_set.find(dst_address) == excluded_set.end())
					addresses->push_back(dst_address);
			}
		}

		// Sort by names
		addresses->sort();

		return addresses;
	}


	ServiceListPtr RuleList::all_services(const std::set<const Service*>& excluded_set) const
	{
		ServiceListPtr services = std::make_unique<ServiceList>();

		for (const Rule* rule : _rules) {
			const Predicate& predicate = rule->predicate();

			for (const Service* service : predicate.services().items()) {
				if (excluded_set.find(service) == excluded_set.end())
					services->push_back(service);
			}
		}

		// Sort by names
		services->sort();

		return services;
	}


	ApplicationListPtr RuleList::all_applications(const std::set<const Application*>& excluded_set) const
	{
		ApplicationListPtr applications = std::make_unique<ApplicationList>();

		for (const Rule* rule : _rules) {
			const Predicate& predicate = rule->predicate();

			for (const Application* application : predicate.applications().items()) {
				if (excluded_set.find(application) == excluded_set.end())
					applications->push_back(application);
			}
		}

		// Sort by names
		applications->sort();

		return applications;
	}


	UserListPtr RuleList::all_users(const std::set<const User*>& excluded_set) const
	{
		UserListPtr users = std::make_unique<UserList>();

		for (const Rule* rule : _rules) {
			const Predicate& predicate = rule->predicate();

			for (const User* user : predicate.users().items()) {
				if (excluded_set.find(user) == excluded_set.end())
					users->push_back(user);
			}
		}

		// Sort by names
		users->sort();

		return users;
	}


	UrlListPtr RuleList::all_urls(const std::set<const Url*>& excluded_set) const
	{
		UrlListPtr urls = std::make_unique<UrlList>();

		for (const Rule* rule : _rules) {
			const Predicate& predicate = rule->predicate();

			for (const Url* url : predicate.urls().items()) {
				if (excluded_set.find(url) == excluded_set.end())
					urls->push_back(url);
			}
		}

		// Sort by names
		urls->sort();

		return urls;
	}


	bool RuleList::is_last(const Rule* rule) const
	{
		return rule == _rules.back();
	}


	std::vector<int> RuleList::id_list() const
	{
		std::vector<int> id_list;
		id_list.reserve(this->size());

		for (const Rule* rule : _rules)
			id_list.push_back(rule->id());

		return id_list;
	}


	bool RuleList::have_names() const
	{
		return std::any_of(
					_rules.cbegin(),
					_rules.cend(),
					[](const Rule* rule) { return !rule->name().empty(); }
		);
	}


	bool RuleList::have_negate() const
	{
		return std::any_of(
			_rules.cbegin(),
			_rules.cend(),
			[](const Rule* rule) {
				const Predicate& predicate = rule->predicate();
				return predicate.negate_src_addresses() || predicate.negate_dst_addresses();
			}
		);
	}


	RuleList::Counters RuleList::get_counters() const
	{
		Counters counters{ 0, 0 };

		for (const Rule* rule : _rules) {
			if (rule->action() == RuleAction::ALLOW)
				counters.allowed++;
			else
				counters.denied++;
		}

		return counters;
	}


	Table RuleList::create_table(const RuleOutputOptions& options) const
	{
		Table::Headers headers;

		// define the headers
		headers.push_back("id");
		if (options.contains(RuleOutputOption::RULE_NAME))
			headers.push_back("name");
		headers.push_back("action");
		headers.push_back("src.zone");
		headers.push_back("dst.zone");
		if (options.contains(RuleOutputOption::NEGATE_ADDRESS))
			headers.push_back("src.negate");
		if (options.contains(RuleOutputOption::ADDRESS_NAME))
			headers.push_back("src.addr");
		headers.push_back("src.ip");
		if (options.contains(RuleOutputOption::NEGATE_ADDRESS))
			headers.push_back("dst.negate");
		if (options.contains(RuleOutputOption::ADDRESS_NAME))
			headers.push_back("dst.addr");
		headers.push_back("dst.ip");
		if (options.contains(RuleOutputOption::SERVICE_NAME))
			headers.push_back("svc name");
		headers.push_back("svc");
		if (options.contains(RuleOutputOption::APPLICATION_NAME))
			headers.push_back("app");
		if (options.contains(RuleOutputOption::USER_NAME))
			headers.push_back("usr");
		if (options.contains(RuleOutputOption::URL))
			headers.push_back("url");

		// allocate and initialize the table
		Table table{ headers };

		for (const Rule* rule : _rules) {
			rule->write_to_row(table.add_row(), options);
		}

		return table;
	}


	std::string RuleList::Counters::to_string() const
	{
		return std::to_string(allowed) + "/" + std::to_string(denied);
	}

}
