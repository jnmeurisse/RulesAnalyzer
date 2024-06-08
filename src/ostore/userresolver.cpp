/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#include "userresolver.h"


namespace fos {

	UserResolver::UserResolver(const UserObjects& objects, const UserPools& pools) :
		GenericResolver(objects, pools)
	{
	}

}
