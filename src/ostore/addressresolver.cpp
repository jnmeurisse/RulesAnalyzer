/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#include "addressresolver.h"

namespace fos {

	AddressResolver::AddressResolver(const AddressObjects& objects, const AddressPools& pools) :
		GenericResolver(objects, pools)
	{
	}

}
