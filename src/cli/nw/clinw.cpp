/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#include "cli/nw/clinw.h"

#include "cli/nw/clinwlist.h"
#include "model/firewall.h"


namespace cli {

	CliNwCommand::CliNwCommand(CliContext& context) :
		CliCommandMap(context)
	{
		// Create a default firewall
		context.add_firewall("default");

		add(CommandKeys{ "l", "list" }, new CliNwListCommand(context));
	}

}
