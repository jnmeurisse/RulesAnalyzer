/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#pragma once

#include "ostore/genericresolver.h"
#include "ostore/userobject.h"
#include "ostore/userpool.h"


namespace fos {

	class UserResolver : public GenericResolver<UserObject, UserPool>
	{
	public:
		using GenericResolver<UserObject, UserPool>::GenericResolver;
	};

}
