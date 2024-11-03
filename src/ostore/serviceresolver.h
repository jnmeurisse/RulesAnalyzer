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
#include "ostore/serviceobject.h"
#include "ostore/servicepool.h"


namespace fos {

	class ServiceResolver : public GenericResolver<ServiceObject, ServicePool>
	{
	public:
		using GenericResolver<ServiceObject, ServicePool>::GenericResolver;
	};

}
