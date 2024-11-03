/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#pragma once
#include "global.h"

#include <tuple>

#include "model/mnode.h"
#include "model/rulelist.h"

namespace fwm {

	struct PolicylistRelationShip {
		MnodeRelationship allowed;
		MnodeRelationship denied;
	};


	class PolicyListComparator {
	public:
		static PolicylistRelationShip compare(const RuleList& rule_list1, const RuleList& rule_list2);

	private:
		static std::tuple<Bddnode, Bddnode> compute_bdd(const RuleList& rule_list);
	};

}
