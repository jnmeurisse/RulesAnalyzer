/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#include "clinw.h"

#include "cli/nw/clinwlist.h"

#include "model/firewall.h"
#include "ostore/firewallfactory.h"


namespace cli {

	CliNwCommand::CliNwCommand(CliContext& context) :
		CliCommandMap(context)
	{
		FirewallPtr firewall = std::make_unique<Firewall>(
			"default",
			context.nw
			);

		context.nw.add(firewall);
		context.fw = context.nw.get("default");

		add(CommandKeys{ "l", "list" }, new CliNwListCommand(context));
	};

}
