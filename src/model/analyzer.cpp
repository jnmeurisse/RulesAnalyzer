/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#include "model/analyzer.h"

#include <cstdio>
#include <functional>
#include <memory>
#include <utility>

#include "model/domains.h"
#include "model/gbchandler.h"
#include "model/state.h"


namespace fwm {

	Analyzer::Analyzer(const RuleList& rule_list, IPAddressModel ip_model) :
		_acl{ rule_list },
		_ip_model{ ip_model }
	{
	}


	RuleList Analyzer::check_any(const DstAddressGroup& any_addresses) const
	{
		auto select_func = [&any_addresses](const Rule& rule) -> bool {
			const Predicate& predicate{ rule.predicate() };
			return
				rule.action() == RuleAction::ALLOW &&
				any_addresses.is_subset(predicate.dst_addresses().negate_if(predicate.negate_dst_addresses())) &&
				predicate.services().is_any();
		};

		return _acl.filter(select_func);
	}


	RuleList Analyzer::check_deny() const
	{
		RuleList any_rules;

		PredicatePtr any_predicate = std::unique_ptr<Predicate>(Predicate::any(_ip_model));

		for (const Rule* rule : _acl) {
			if (rule->action() == RuleAction::DENY && rule->predicate().equal(*any_predicate)) {
				any_rules.push_back(rule);
			}
		}

		return any_rules;
	}


	std::list<RulePair> Analyzer::check_symmetry(bool strict, f_interrupt_cb interrupt_cb) const
	{
		std::list<RulePair> symmetrical_rules;

		int rule_index{ 0 };
		for (const Rule* rule : _acl) {
			auto it = _acl.begin();
			std::advance(it, ++rule_index);

			for (; it != _acl.end(); ++it) {
				if (interrupt_cb())
					throw interrupt_error("** interrupted **");

				if (rule->action() == (*it)->action() &&
					rule->predicate().is_symmetrical((*it)->predicate(), strict)) {
					symmetrical_rules.push_back(std::make_tuple(rule, *it));
				}
			}
		}

		return symmetrical_rules;
	}


	RuleAnomalies Analyzer::check_anomaly(f_interrupt_cb interrupt_cb) const
	{
		RuleAnomalies anomalies{};

		// Initial state is configured to accept any packets
		PredicatePtr any_predicate = std::unique_ptr<Predicate>(Predicate::any(_ip_model));
		State state{ *any_predicate };

		// The bdd cache is mostly used when searching for preceding rules that are
		// involved in an anomaly.  It avoids to rebuild the bdd over and over.
		Bddcache cache{};
		cache.reserve(_acl.size());

		// Initialize the progress bar.
		bool show_progress = _acl.size() > 20;
		int loop_counter = 0;
		GbcHandler gbc_handler(show_progress);

		for (const Rule* rule : _acl) {
			if (interrupt_cb())
				throw interrupt_error("** interrupted **");

			// Compute the bdd of the current rule and add it to the local cache.
			cache[rule->id()] = Bddnode{ rule->predicate().make_bdd() };

			if (!(rule->is_deny_all() && _acl.is_last(rule))) {
				// Check for anomalies.
				RuleAnomaly* anomaly = check_rule(*rule, state, cache);
				if (anomaly)
					anomalies.push_back(std::unique_ptr<RuleAnomaly>(anomaly));
			}

			// Update the state of the analyzer.
			state.update(rule->action(), cache.at(rule->id()));

			if (show_progress) {
				// Show progress
				if ((++loop_counter % 100 == 0))
					std::cout << '*' << std::flush;
				else if (loop_counter % 10 == 0)
					std::cout << '+' << std::flush;
				else
					std::cout << '.' << std::flush;
			}
		}

		anomalies.missing_deny_all = !state.remaining().is_none();

		if (show_progress)
			std::cout << std::endl;

		return anomalies;
	}


	RuleAnomaly* Analyzer::check_rule(const Rule& rule, const State& state, const Bddcache& cache) const
	{
		RuleAnomalyDetails* anomaly_details;

		// Compare the rule with the set of remaining potential packets.
		const Bddnode& predicate_bdd = cache.at(rule.id());
		if (predicate_bdd.is_subset(state.remaining())) {
			/*               This is a good rule               */
			/***************************************************/
			anomaly_details = nullptr;
		}
		else if (state.remaining().is_none() || predicate_bdd.is_disjoint(state.remaining())) {
			/*               This is a fully masked rule       */
			/***************************************************/
			anomaly_details = analyze_fully_masked_rule(rule, state, cache);
		}
		else {
			/*               This is a partially masked rule   */
			/***************************************************/
			anomaly_details = analyze_partially_masked_rule(rule, state, cache);
		}

		return anomaly_details ? new RuleAnomaly(rule, anomaly_details) : nullptr;
	}


	RuleAnomalyDetails* Analyzer::analyze_fully_masked_rule(const Rule& rule, const State& state, const Bddcache& cache) const
	{
		const Bddnode& predicate_bdd = cache.at(rule.id());

		if (predicate_bdd.is_subset(state.processed(!rule.action()))) {
			// Shadowed by preceding deny(/allow) rules. Find a deny(/allow) rule
			// that completely hides this rule or a combination of deny(/allow) rules
			// that globally hide this rule.
			return new RuleAnomalyShadowed(find_overlaping(rule, !rule.action(), cache));
		}

		if (predicate_bdd.is_disjoint(state.processed(!rule.action()))) {
			// Redundant by preceding allow(/deny) rules.
			return new RuleAnomalyFullRedundant(find_overlaping(rule, rule.action(), cache));
		}

		// Redundant or correlated rules.

		// Part of the packets intended to be accepted by this rule have been
		// denied(/allowed) by preceding rules.
		RuleList correlated_rules = find_overlaping(rule, !rule.action(), cache);

		// Other packets have been accepted.
		// Find an allow(/deny) rule that completely hides this rule or a
		// a combination of allow(/deny) rules that globally hide this rule.
		RuleList redundant_rules = find_overlaping(rule, rule.action(), cache);

		return new RuleAnomalyRedundantOrCorrelated(
				redundant_rules,
				correlated_rules
			);
	};


	RuleAnomalyDetails* Analyzer::analyze_partially_masked_rule(const Rule& rule, const State& state, const Bddcache& cache) const
	{
		const Bddnode& predicate_bdd = cache.at(rule.id());

		// Search for a generalization rule
		RuleList matching_rules = find_other_is_subset(rule, !rule.action(), cache);
		if (matching_rules.size() > 0) {
			return new RuleAnomalyGeneralization(matching_rules);
		}

		// Search for redundancy
		if (predicate_bdd.overlaps(state.processed(rule.action()))) {
			matching_rules = find_other_is_subset(rule, rule.action(), cache);
			if (matching_rules.size() > 0) {
				return new RuleAnomalyPartialRedundant(matching_rules);
			}
		}

		// Search for a correlated rule
		if (predicate_bdd.overlaps(state.processed(!rule.action()))) {
			matching_rules = find_overlaping(rule, !rule.action(), cache);
			if (matching_rules.size() > 0) {
				return new RuleAnomalyCorrelated(matching_rules);
			}
		}

		return nullptr;
	}


	RuleList Analyzer::find_is_subset(const Rule& rule, RuleAction action, const Bddcache& cache) const
	{
		const Bddnode& predicate_bdd{ cache.at(rule.id()) };

		auto select_func = [action, &predicate_bdd, cache](const Rule& other) -> bool {
			return other.action() == action && predicate_bdd.is_subset(cache.at(other.id()));
		};

		return _acl.filter_before(&rule, select_func);
	};


	RuleList Analyzer::find_other_is_subset(const Rule& rule, RuleAction action, const Bddcache& cache) const
	{
		const Bddnode& predicate_bdd{ cache.at(rule.id()) };

		auto select_func = [action, &predicate_bdd, cache](const Rule& other) -> bool {
			return other.action() == action && cache.at(other.id()).is_subset(predicate_bdd);
		};

		return _acl.filter_before(&rule, select_func);
	}


	RuleList Analyzer::find_overlaping(const Rule& rule, RuleAction action, const Bddcache& cache) const
	{
		const Bddnode& predicate_bdd{ cache.at(rule.id()) };

		auto select_func = [action, &predicate_bdd, cache](const Rule& other) -> bool {
			return other.action() == action && predicate_bdd.overlaps(cache.at(other.id()));
		};

		return _acl.filter_before(&rule, select_func);
	}

}
