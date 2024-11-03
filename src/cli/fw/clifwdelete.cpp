/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#include "cli/fw/clifwdelete.h"


namespace cli {

	CliFwDeleteCommand::CliFwDeleteCommand(CliContext& context) :
		CliCommand(context, 1, 1, new CliCommandFlags())
	{
	};


	void CliFwDeleteCommand::do_execute(CliArgs& args, const CliCtrlcGuard& ctrlc_guard)
	{
		const std::string firewall_name{ args.pop() };

		context.delete_firewall(firewall_name);
		context.logger->info("firewall '%s' deleted", firewall_name.c_str());
	}

}
