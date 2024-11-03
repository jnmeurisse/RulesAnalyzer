/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#pragma once
#include "global.h"

#include "ostore/genericresolver.h"
#include "ostore/applicationobject.h"
#include "ostore/applicationpool.h"


namespace fos {

	class ApplicationResolver : public GenericResolver<ApplicationObject, ApplicationPool>
	{
	public:
		using GenericResolver<ApplicationObject, ApplicationPool>::GenericResolver;
	};

}
