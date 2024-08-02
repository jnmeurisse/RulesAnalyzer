/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#include "domain.h"

#include <cassert>

namespace fwm {

	Domain::Domain(DomainType dt, uint16_t nbits) :
		_dt{ dt },
		_nbits{ nbits }
	{
		assert(nbits > 0 && nbits <= 32);
	}


	size_t Domain::size() const
	{
		const size_t mask{ 1 };
		return mask << _nbits;
	}


	uint32_t Domain::upper() const
	{
		// cast is safe because
		//   nbits > 0 => size() >= 2 => size()-1 >= 1 
		//   nbits <= 32 => size() <= UINT_MAX+1 => size()-1 <= UINT_MAX
		return static_cast<uint32_t>(size() - 1);
	}

}
