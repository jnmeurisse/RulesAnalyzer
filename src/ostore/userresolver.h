/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#pragma once
#include "userobject.h"
#include "userobjects.h"
#include "userpool.h"
#include "userpools.h"
#include "genericresolver.h"


namespace fos {

	class UserResolver : public GenericResolver<UserObject, UserPool>
	{
	public:
		UserResolver(const UserObjects& objects, const UserPools& pools);
	};

}
