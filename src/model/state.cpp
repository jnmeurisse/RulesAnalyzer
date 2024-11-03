/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#include "model/state.h"

#include <stdexcept>


namespace fwm {

	State::State(const Predicate& predicate) :
		_predicate{ predicate },
		_I{ predicate.make_bdd() },
		_A{},
		_D{},
		_R{ StateVar{ predicate.make_bdd() } }
	{
	}


	void State::update(RuleAction action, const Bddnode& predicate)
	{
		if (action == RuleAction::ALLOW) {
			// A <- A Union(R Inter P)
			_A = StateVar(_A.make_bdd() | (_R.make_bdd() & predicate.make_bdd()));
		}
		else if (action == RuleAction::DENY) {
			// D <- D Union(R inter P)
			_D = StateVar(_D.make_bdd() | (_R.make_bdd() & predicate.make_bdd()));
		}
		else
			throw std::runtime_error("internal error : invalid rule action");

		// Update R 
		// R <- I Inter Not(A Union D)
		_R = StateVar(bdd_and(_I.make_bdd(), bdd_not(bdd_or(_A.make_bdd(), _D.make_bdd()))));
	}


	const StateVar& State::processed(RuleAction action) const noexcept
	{
		return (action == RuleAction::ALLOW) ? _A : _D;
	}

}
