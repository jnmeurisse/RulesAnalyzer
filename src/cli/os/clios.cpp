/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#include "clios.h"

#include "cli/os/cliosclear.h"
#include "cli/os/cliosload.h"
#include "cli/os/cliosinfo.h"
#include "cli/os/cliosquery.h"

namespace cli {

	CliOsCommand::CliOsCommand(CliContext& context) :
		CliCommandMap(context)
	{
		add(CommandKeys{ "c", "clear" }, new CliOsClearCommand(context));
		add(CommandKeys{ "i", "info" },  new CliOsInfoCommand(context));
		add(CommandKeys{ "l", "load" },  new CliOsLoadCommand(context));
		add(CommandKeys{ "q", "query"},  new CliOsQueryCommand(context));
	}

}
