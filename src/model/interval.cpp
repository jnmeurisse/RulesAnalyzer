/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#include <cassert>
#include <string>

#include "domains.h"
#include "interval.h"

namespace fwm {

	Interval::Interval(const DomainType dt, uint32_t lower, uint32_t upper) :
		Mnode(),
		_domain{ Domains::get(dt) },
		_lower{ lower },
		_upper{ upper }
	{
		assert(upper <= _domain.upper());
	}


	bdd Interval::make_bdd() const
	{
		const int nb{ _domain.nbits() };
		const bvec& domain_var{ _domain.var() };

		if (size() == 1) {
			// lower and upper bounds are equal, we test var == lbound
			const bvec v{ nb, _lower };
			return bvec_equ(domain_var, v);
		}
		else if (size() == _domain.size()) {
			return bddtrue;
		}
		else {
			// we test lbound <= var <= ubound
			const bvec l{ nb, _lower };
			const bvec u{ nb, _upper };

			const bdd condition1{ bvec_lte(l, domain_var) };
			const bdd condition2{ bvec_lte(domain_var, u) };

			return bdd_and(condition1, condition2);
		}
	}


	size_t Interval::size() const noexcept
	{
		return (_lower > _upper) ? 0 : _upper - _lower + 1LL;
	}


	std::string Interval::to_string() const
	{
		if (_lower == _upper)
			return std::to_string(_lower);
		else
			return std::to_string(_lower) + "-" + std::to_string(_upper);
	}

}
