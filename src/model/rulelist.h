/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#pragma once
#include "global.h"

#include <functional>
#include <set>
#include <vector>

#include "model/rule.h"
#include "model/table.h"


namespace fwm {

	class RuleList
	{
	public:
		/**
		 * Allocates an empty list.
		*/
		RuleList() = default;

		/**
		 * Allocates an empty list with a reserved size.
		*/
		RuleList(size_t default_size);

		/**
		 * Appends a rule to this list.
		 *
		 * @param rule A pointer to a rule.
		 */
		void push_back(const Rule* rule);

		/**
		 * Rules iterator
		 */
		using const_iterator = std::vector<const Rule*>::const_iterator;
		const_iterator begin() const;
		const_iterator end() const;

		/**
		 * Returns the first rule
		 */
		const Rule* front() const;

		/**
		 * Returns the last rule
		 */
		const Rule* back() const;

		/**
		 * Returns the number of rules in this list.
		 */
		size_t size() const;

		/**
		 * Clears the content of the list.
		 */
		void clear();

		/* Returns a list of rules satisfying the given predicate.
		*/
		RuleList filter(std::function<bool(const Rule&)> predicate) const;

		/* Returns a list of rules having the specified action.
		*/
		RuleList filter(RuleAction action_filter) const;

		/* Returns a list of rules having the specified status.
		*/
		RuleList filter(RuleStatus status_filter) const;

		/* Returns a list of rules satisfying the source and destination zones criteria.
		*/
		RuleList filter(const SrcZone& src_zone, const DstZone& dst_zone) const;

		/* Returns a list of rules satisfying the source and destination zones criteria.
		*/
		RuleList filter(const ZonePair& zones) const;

		/* Returns all rules using the given source address.
		*/
		RuleList filter(const SrcAddress* address) const;

		/* Returns all rules using the given destination address.
		*/
		RuleList filter(const DstAddress* address) const;

		/* Returns all rules using the given service
		*/
		RuleList filter(const Service* service) const;

		/* Returns all rules using the given application.
		*/
		RuleList filter(const Application* application) const;

		/* Returns all rules using the given user.
		*/
		RuleList filter(const User* user) const;

		/* Returns all rules using the given url.
		*/
		RuleList filter(const Url* url) const;

		/* Returns all rules allowing all given services.
		*/
		RuleList filter(const ServiceGroup& services) const;

		/* Returns all rules allowing all given applications and optionally all services when
		*  the services group is not empty.
		*/
		RuleList filter(const ApplicationGroup& applications, const ServiceGroup& services) const;

		/* Returns a list of rules found before the rule sentinel and satisfying
		*  the given predicate.
		*/
		RuleList filter_before(const Rule* sentinel, std::function<bool(const Rule&)> predicate) const;

		/* Returns all zones sorted by name except those defined in the exclude_set parameter.
		*/
		ZoneListPtr all_zones(const std::set<const Zone*> excluded_set = {}) const;

		/* Returns all source zones sorted by name except those defined in the exclude_set parameter.
		*/
		SrcZoneListPtr all_src_zones(const std::set<const SrcZone*>& excluded_set = {}) const;

		/* Returns all destination zones sorted by name except those defined in the exclude_set parameter.
		*/
		DstZoneListPtr all_dst_zones(const std::set<const DstZone*>& excluded_set = {}) const;

		/* Returns all addresses sorted by name except those defined in the exclude_set parameter.
		*/
		AddressListPtr all_addresses(const std::set<const IpAddress*>& excluded_set = {}) const;

		/* Returns all services sorted by name except those defined in the exclude_set parameter.
		*/
		ServiceListPtr all_services(const std::set<const Service*>& excluded_set = {}) const;

		/* Returns all applications sorted by name except those defined in the exclude_set parameter.
		*/
		ApplicationListPtr all_applications(const std::set<const Application*>& excluded_set = {}) const;

		/* Returns all users sorted by name except those defined in the exclude_set parameter.
		*/
		UserListPtr all_users(const std::set<const User*>& excluded_set = {}) const;

		/* Returns all urls sorted by name except those defined in the exclude_set parameter.
		*/
		UrlListPtr all_urls(const std::set<const Url*>& excluded_set = {}) const;

		/* Returns true if the given rule is the last of this list.
		*/
		bool is_last(const Rule* rule) const;

		/* Returns the list of rule id.
		*/
		std::vector<int> id_list() const;

		/* Returns true if at least one rule has a name.
		*/
		bool have_names() const;

		/* Returns true if at least one rule has a negate source or destination addresses.
		 */
		bool have_negate() const;

		/* Counters of allowed or denied rules.
		*/
		struct Counters {
			int allowed;
			int denied;

			std::string to_string() const;
		};

		/* Returns the number of allowed/denied rules in this list.
		*/
		Counters get_counters() const;

		/* Creates a table from this list of rules.
		*/
		Table create_table(const RuleOutputOptions& options) const;


	private:
		std::vector<const Rule*> _rules;
	};

}
