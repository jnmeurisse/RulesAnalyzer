/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#pragma once

#include "ostore/genericresolver.h"
#include "ostore/urlobject.h"
#include "ostore/urlpool.h"


namespace fos {

	class UrlResolver : public GenericResolver<UrlObject, UrlPool>
	{
	public:
		using GenericResolver<UrlObject, UrlPool>::GenericResolver;
	};

}
