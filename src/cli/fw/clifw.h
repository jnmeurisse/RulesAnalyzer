/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#pragma once
#include "global.h"

#include "cli/clicmdmap.h"
#include "cli/clicontext.h"


namespace cli {

	class CliFwCommand : public CliCommandMap
	{
	public:
		CliFwCommand(CliContext& context);
	};

}
