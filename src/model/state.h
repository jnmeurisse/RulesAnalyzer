/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#pragma once
#include "global.h"

#include <buddy/bdd.h>

#include "model/mnode.h"
#include "model/predicate.h"
#include "model/rule.h"


namespace fwm {

	using StateVar = Bddnode;

	class State
	{
	public:
		State(const Predicate& predicate);
		void update(RuleAction action, const Bddnode& predicate);

		inline const StateVar& input() const noexcept { return _I; }
		inline const StateVar& accepted() const noexcept { return _A; }
		inline const StateVar& remaining() const noexcept { return _R; }
		inline const StateVar& denied() const noexcept { return _D; }
		const StateVar& processed(RuleAction action) const noexcept;

	private:
		const Predicate& _predicate;

		StateVar _I;
		StateVar _A;
		StateVar _D;
		StateVar _R;
	};

}
