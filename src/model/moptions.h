/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#pragma once
#include "global.h"

#include "tools/options.h"


namespace fwm {

	enum class ModelOption {
		Application,
		User,
		Url
	};


	class ModelOptions : public rat::Options<ModelOption>
	{
	public:
		ModelOptions() = default;

		static const ModelOptions& empty();
	};

}
