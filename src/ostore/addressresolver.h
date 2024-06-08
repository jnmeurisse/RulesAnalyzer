/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#pragma once

#include "addressobject.h"
#include "addressobjects.h"
#include "addresspool.h"
#include "addresspools.h"
#include "genericresolver.h"


namespace fos {

	class AddressResolver : public GenericResolver<AddressObject, AddressPool>
	{
	public:
		AddressResolver(const AddressObjects& objects, const AddressPools& pools);
	};

}
