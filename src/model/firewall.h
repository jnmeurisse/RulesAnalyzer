/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#pragma once
#include "global.h"

#include <map>
#include <string>
#include <utility>
#include <vector>
#include "model/rule.h"
#include "model/rulelist.h"
#include "model/table.h"
#include "tools/interrupt.h"


namespace fwm {

	/**
	 * The Network class is the owner of all firewalls.
	*/
	class Network;


	/**
	 *
	*/
	class Firewall final
	{
	public:
		Firewall(const std::string& name, Network& network);
		Firewall(const Firewall& other, const std::string& name);

		/* Clears all rules
		*/
		virtual void clear();

		/* Returns the access control list including only enabled rules.
		*/
		const RuleList acl() const noexcept;

		/* Returns a pointer to a rule having the specified rule id.
		 * The method returns null if not found.
		*/
		Rule* get_rule(int rule_id);
		const Rule* get_rule(int rule_id) const;

		/* Writes all rules to a table.
		*/
		Table create_rules_table(const RuleOutputOptions& output_options) const;

		/* Returns the firewall name.
		*/
		inline const std::string& name() const noexcept { return _name; }

		/* Returns the network containing this firewall.
		*/
		inline Network& network() const noexcept { return _network; }

		/* Returns rule output options
		*/
		RuleOutputOptions make_output_options(bool show_object_name) const;

		/* Adds a rule to this firewall.  The rule ownership is transfered
		 * from the caller to this firewall object
		 */
		void add_rule(Rule* rule);

		/*
		* Returns a summary of the rules.
		*/
		Table info() const;


	private:
		// The firewall name
		const std::string _name;

		// The network containing this firewall
		Network& _network;

		// All rules
		std::vector<RulePtr> _rules;

		// Firewall acl
		RuleList _rule_list;

		// All rules by id
		std::map<int, Rule*> _rule_ids;
	};

	using FirewallPtr = std::unique_ptr<Firewall>;

}
