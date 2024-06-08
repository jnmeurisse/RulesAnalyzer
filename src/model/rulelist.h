/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#pragma once
#include <functional>
#include <vector>

#include "rule.h"
#include "table.h"

namespace fwm {

	class RuleList : public std::list<const Rule*>
	{
	public:
		RuleList() = default;

		/* Returns a list of rules satisfying the given predicate.
		*/
		RuleList filter(std::function<bool(const Rule&)> predicate) const;

		/* Returns a list of rules satisfying the given predicate if a condition is set
		*  else returns a copy of this list.
		*/
		RuleList filter_if(std::function<bool(const Rule&)> predicate, bool condition) const;

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
		 * if specified else returns a copy of this list.
		*/
		RuleList filter_if(const ZonePair& zones) const;

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

		/* Returns all zone names.
		*/
		ZoneNames zone_names() const;

		/* Returns all source zone names.
		*/
		ZoneNames src_zone_names() const;

		/* Returns all destination zone names.
		*/
		ZoneNames dst_zone_names() const;

		/* Returns all source addresses.
		*/
		SrcAddressGroupPtr source_addresses() const;

		/* Returns all destination addresses.
		*/
		DstAddressGroupPtr destination_addresses() const;

		/* Returns all addresses.
		*/
		AddressGroupPtr addresses() const;

		/* Returns all services.
		*/
		ServiceGroupPtr services() const;

		/* Returns all applications.
		*/
		ApplicationGroupPtr applications() const;

		/* Return all users
		*/
		UserGroupPtr users() const;

		/* Returns true if the given rule is the last of this list.
		*/
		bool is_last(const Rule* rule) const;

		/* Returns the list of rule id.
		*/
		std::vector<int> id_list() const;

		/* Returns true if at least one rule is named.
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
		Table output(const RuleOutputOptions& options) const;
	};

}
