/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#include "clifwcopy.h"

namespace cli {
	
	CliFwCopyCommand::CliFwCopyCommand(CliContext& context) :
		CliCommand(context, 1, 1, new CliCommandFlags())
	{
	}


	void CliFwCopyCommand::do_execute(CliArgs& args, const CliCtrlcGuard& ctrlc_guard)
	{
		const Firewall* firewall = context.fw;
		if (!firewall)
			report_firewall_not_selected();

		const std::string firewall_name{ args.pop() };

		if (context.nw.get(firewall_name)) {
			context.logger->error("firewall '%s' already exists", firewall_name.c_str());
		}
		else {
			FirewallPtr firewall_copy{ context.ff.clone(*firewall, firewall_name) };
			context.nw.add(firewall_copy);

			context.logger->info("firewall '%s' copied to '%s'",
				firewall->name().c_str(),
				firewall_name.c_str()
			);
		}
	}
}
