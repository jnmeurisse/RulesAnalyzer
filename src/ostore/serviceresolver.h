/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#pragma once

#include "genericresolver.h"
#include "serviceobject.h"
#include "serviceobjects.h"
#include "servicepools.h"


namespace fos {

	class ServiceResolver : public GenericResolver<ServiceObject, ServicePool>
	{
	public:
		ServiceResolver(const ServiceObjects& objects, const ServicePools& pools);
	};

}
