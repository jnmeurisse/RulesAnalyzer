/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#pragma once

#include "address.h"
#include "cache.h"

namespace fwm {

	class SrcAddressCache : public Cache<SrcAddress>
	{
	public:
		SrcAddressCache() = default;
	};


	class SrcAddressGroupCache : public Cache<SrcAddressGroup>
	{
	public:
		SrcAddressGroupCache() = default;
	};


	class DstAddressCache : public Cache<DstAddress>
	{
	public:
		DstAddressCache() = default;
	};


	class DstAddressGroupCache : public Cache<DstAddressGroup>
	{
	public:
		DstAddressGroupCache() = default;
	};

}
