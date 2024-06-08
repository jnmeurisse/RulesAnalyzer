/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#pragma once

#include "applicationobject.h"
#include "applicationobjects.h"
#include "applicationpool.h"
#include "applicationpools.h"
#include "genericresolver.h"


namespace fos {

	class ApplicationResolver : public GenericResolver<ApplicationObject, ApplicationPool>
	{
	public:
		ApplicationResolver(const ApplicationObjects& objects, const ApplicationPools& pools);
	};

}
