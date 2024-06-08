/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#include "applicationresolver.h"

namespace fos {

	ApplicationResolver::ApplicationResolver(const ApplicationObjects& objects, const ApplicationPools& pools) :
		GenericResolver(objects, pools)
	{
	}

}
