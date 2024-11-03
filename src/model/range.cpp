/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#include "model/range.h"

#include <cassert>


namespace fwm {

	Range::Range(int nbits) :
		_nbits{ nbits }
	{
		assert(nbits >= 0 && nbits <= 128);
	}


	bool Range::operator==(const Range& other) const
	{
		return ((lbound() == other.lbound()) == bddtrue) && ((ubound() == other.ubound()) == bddtrue);
	}

}
