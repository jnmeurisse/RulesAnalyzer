/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#include "cli/fw/clifwcreate.h"

#include "model/firewall.h"


namespace cli {

	CliFwCreateCommand::CliFwCreateCommand(CliContext& context) :
		CliCommand(context, 1, 1, new CliCommandFlags())
	{
	};


	void CliFwCreateCommand::do_execute(CliArgs& args, const CliCtrlcGuard& ctrlc_guard)
	{
		const Firewall& firewall = context.add_firewall(args.pop());
		context.logger->info("firewall '%s' created", firewall.name().c_str());
	}

}
