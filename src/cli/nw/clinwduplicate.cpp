/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#include "cli/nw/clinwduplicate.h"


namespace cli {
	CliNwDuplicateCommand::CliNwDuplicateCommand(CliContext& context) :
		CliCommandMap(context)
	{
	}


	CliNwAdDuplicateCommand::CliNwAdDuplicateCommand(CliContext& context) :
		CliCommand(context)
	{
	}

}
