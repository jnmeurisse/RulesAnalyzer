/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#pragma once
#include "global.h"

#include "ostore/addressobject.h"
#include "ostore/addresspool.h"
#include "ostore/genericresolver.h"


namespace fos {

	class AddressResolver : public GenericResolver<AddressObject, AddressPool>
	{
	public:
		using GenericResolver<AddressObject, AddressPool>::GenericResolver;
	};

}
