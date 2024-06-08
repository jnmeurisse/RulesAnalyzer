/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#pragma once

#include "cache.h"
#include "user.h"

namespace fwm {

	class UserCache : public Cache<User>
	{
	};


	class UserGroupCache : public Cache<UserGroup>
	{
	};

}
