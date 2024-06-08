/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#pragma once

#include <string>
#include <vector>
#include "poolobject.h"


namespace fos {

	class ApplicationPool : public PoolObject
	{
	public:
		ApplicationPool(const std::string& name, const std::vector<std::string>& members);
	};

}
