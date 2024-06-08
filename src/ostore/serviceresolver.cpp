/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#include "serviceresolver.h"


namespace fos {

	ServiceResolver::ServiceResolver(const ServiceObjects& objects, const ServicePools& pools) :
		GenericResolver(objects, pools)
	{
	}

}
