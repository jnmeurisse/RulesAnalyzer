/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#include "cli/fw/clifwcopy.h"

#include <string>


namespace cli {

	CliFwCopyCommand::CliFwCopyCommand(CliContext& context) :
		CliCommand(context, 1, 1, new CliCommandFlags())
	{
	}


	void CliFwCopyCommand::do_execute(CliArgs& args, const CliCtrlcGuard& ctrlc_guard)
	{
		// get destination firewall name
		const std::string firewall_name{ args.pop() };

		// make a copy of the current firewall and store it in the network
		context.clone_current_firewall(firewall_name);

		context.logger->info("firewall '%s' copied to '%s'",
			context.get_current_firewall().name().c_str(),
			firewall_name.c_str()
		);
	}

}
