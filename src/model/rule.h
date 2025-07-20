/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#pragma once
#include "global.h"

#include <memory>
#include <list>
#include <string>
#include "model/mnode.h"
#include "model/table.h"
#include "model/predicate.h"
#include "tools/options.h"


namespace fwm {

	enum class RuleAction {
		DENY = 0,
		ALLOW = 1
	};

	RuleAction operator!(RuleAction action);

	enum class RuleStatus {
		DISABLED = 0,
		ENABLED = 1
	};

	enum class RuleOutputOption {
		RULE_NAME,                // Output the rule name
		ADDRESS_NAME,             // Output the address name in addition to address IP
		SERVICE_NAME,             // Output the service name
		APPLICATION_NAME,         // Output the application name
		USER_NAME,                // Output the user name
		URL,                      // Output the url
		NEGATE_ADDRESS            // Show negated addresses
	};


	class RuleOutputOptions : public rat::Options<RuleOutputOption>
	{
	public:
		using Options<RuleOutputOption>::Options;
	};


	/**
	* A Firewall class is the owner of all rules.
	*/
	class Firewall;


	/* A security rule.
	*/
	class Rule
	{
	public:
		Rule(Firewall& firewall, const std::string& name, int id, RuleStatus status, RuleAction action, const Predicate* predicate);
		Rule(const Rule& other);

		/* Writes a a representation of this rule to the given table row.
		*/
		void write_to_row(Row& row, const RuleOutputOptions& options) const;

		/* Creates a table representing this rule.
		*/
		Table create_table(const RuleOutputOptions& options) const;

		/* Compares two rules.
		*/
		MnodeRelationship compare(const Rule& other) const;

		/* Returns the rule name.
		*/
		inline const std::string& name() const noexcept { return _name; }

		/* Returns the rule id.
		*/
		inline int id() const noexcept { return _id; }

		/* Returns the rule status (enabled/disabled).
		*/
		inline const RuleStatus status() const noexcept { return _status; }

		/* Returns the firewall action on this rule.
		*/
		inline const RuleAction action() const noexcept { return _action; }

		/* Returns a reference to the rule predicate.
		*/
		inline const Predicate& predicate() const noexcept { return *_predicate; }

		/* Returns true if the rule is configured to use the default application services.
		*/
		bool is_default_app_svc() const;

		/* Returns true if this is a deny all rule.
		*/
		bool is_deny_all() const noexcept;

		/* Update the status of this rule.
		*/
		void set_rule_status(RuleStatus status);

	private:
		// The firewall owning this rule
		Firewall& _firewall;

		// The rule name
		const std::string _name;

		// The rule id
		const int _id;

		// Is this rule enabled or not
		RuleStatus _status;

		// Is it a deny or accept rule.
		const RuleAction _action;

		// Represents the relation between the source, destination, service, application,
		// users that is denied or accepted by this rule.  This rule owns the predicate
		// object.
		const PredicatePtr _predicate;
	};

	using RulePtr = std::unique_ptr<Rule>;
}
