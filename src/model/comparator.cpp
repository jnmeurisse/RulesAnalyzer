/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#include "model/comparator.h"

namespace fwm {

	PolicylistRelationShip PolicyListComparator::compare(const RuleList& rule_list1, const RuleList& rule_list2)
	{
		const auto bdd_list1{ compute_bdd(rule_list1) };
		const auto bdd_list2{ compute_bdd(rule_list2) };

		// Compare allowed (tuple 0) and denied (tuple 1) rules.
		PolicylistRelationShip relations{
			std::get<0>(bdd_list1).compare(std::get<0>(bdd_list2)),
			std::get<1>(bdd_list1).compare(std::get<1>(bdd_list2))
		};

		return relations;
	}


	std::tuple<Bddnode, Bddnode> PolicyListComparator::compute_bdd(const RuleList& rule_list)
	{
		bdd allowed;
		bdd denied;

		for (const Rule* rule : rule_list) {
			if (rule->status() == RuleStatus::ENABLED) {
				if (rule->action() == RuleAction::ALLOW) {
					allowed = allowed | (rule->predicate().make_bdd() - denied);
				}
				else {
					denied = denied | (rule->predicate().make_bdd() - allowed);
				}
			}
		}

		return std::make_tuple<Bddnode, Bddnode>(allowed, denied);
	}

}
