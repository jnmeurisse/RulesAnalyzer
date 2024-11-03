/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#include "model/moptions.h"


namespace fwm {
	
	const ModelOptions& ModelOptions::empty()
	{
		static ModelOptions empty_set;

		return empty_set;
	}
	
}
