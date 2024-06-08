/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#include "clifwdelete.h"

#include <string>

namespace cli {

	CliFwDeleteCommand::CliFwDeleteCommand(CliContext& context) :
		CliCommand(context, 1, 1, new CliCommandFlags())
	{
	};


	void CliFwDeleteCommand::do_execute(CliArgs& args, const CliCtrlcGuard& ctrlc_guard)
	{
		const std::string firewall_name{ args.pop() };

		if (!context.nw.get(firewall_name)) {
			context.logger->error("firewall '%s' not found", firewall_name.c_str());
		}
		else if (context.fw && context.fw->name() == firewall_name) {
			context.logger->error("firewall '%s' is currently selected, you can not delete it", firewall_name.c_str());
		}
		else {
			// delete it from the network
			context.nw.del(firewall_name);
			context.logger->info("firewall '%s' deleted", firewall_name.c_str());
		}
	};

}
