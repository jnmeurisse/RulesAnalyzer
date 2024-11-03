/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#include "model/mvalue.h"


#include <cassert>
#include <string>

#include "model/domains.h"


namespace fwm {

	Mvalue::Mvalue(const DomainType dt, const Range* range) :
		Mnode(),
		_domain{ Domains::get()[dt] },
		_range{ range },
		_all{ *range == _domain.range() }
	{
	}


	bdd Mvalue::make_bdd() const
	{
		if (_all) {
			// Interval range == domain range
			return bddtrue;
		}
		else {
			const bvec& var = _domain.var();

			if (_range->is_singleton()) {
				// lower and upper bounds are equal, we test var == lbound
				return bvec_equ(var, _range->lbound());
			}
			else {
				// we test lbound <= var <= ubound
				const bdd condition1{ bvec_lte(_range->lbound(), var) };
				const bdd condition2{ bvec_lte(var, _range->ubound()) };

				return bdd_and(condition1, condition2);
			}
		}
	}


	std::string Mvalue::to_string() const
	{
		return _range->to_string();
	}

}
