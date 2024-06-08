/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#include "userpool.h"

namespace fos {

	UserPool::UserPool(const std::string& name, const std::vector<std::string>& members) :
		PoolObject(name, members)
	{
	};

}
