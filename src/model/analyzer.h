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
#include <tuple>
#include <unordered_map>

#include "model/anomaly.h"
#include "model/ipaddress.h"
#include "model/rulelist.h"
#include "model/predicate.h"
#include "model/state.h"
#include "tools/interrupt.h"


namespace fwm {

	using RulePair = std::tuple<const Rule*, const Rule*>;

	class Analyzer
	{
	public:
		using Bddcache = std::unordered_map<int, Bddnode>;

		Analyzer(const RuleList& rule_list, IPAddressModel ip_model);

		RuleList check_any(const DstAddressGroup& any_addresses) const;
		RuleList check_deny() const;
		std::list<RulePair> check_symmetry(bool strict, f_interrupt_cb interrupt_cb) const;
		RuleAnomalies check_anomaly(f_interrupt_cb interrupt_cb) const;

		inline const RuleList& acl() const noexcept { return _acl; }

	private:
		const RuleList _acl;
		const IPAddressModel _ip_model;

		RuleAnomaly* check_rule(const Rule& rule, const State& state, const Bddcache& cache) const;
		RuleAnomalyDetails* analyze_fully_masked_rule(const Rule& rule, const State& state, const Bddcache& cache) const;
		RuleAnomalyDetails* analyze_partially_masked_rule(const Rule& rule, const State& state, const Bddcache& cache) const;

		/* Returns all rules having the specified action and where the given rule predicate
		   is a subset of the other rule.
		*/
		RuleList find_is_subset(const Rule& rule, RuleAction action, const Bddcache& cache) const;

		/* Returns all rules having the specified action and where the other rule predicate
		   is a subset of the given rule.
		*/
		RuleList find_other_is_subset(const Rule& rule, RuleAction action, const Bddcache& cache) const;

		/* Returns all rules having the specified action and where the other rule predicate
		   intersects the given rule.
		*/
		RuleList find_overlaping(const Rule& rule, RuleAction action, const Bddcache& cache) const;
	};
}
