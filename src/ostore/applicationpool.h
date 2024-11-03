/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#pragma once
#include "global.h"

#include "ostore/poolobject.h"


namespace fos {

	class ApplicationPool final : public PoolObject
	{
	public:
		using PoolObject::PoolObject;
	};

}
