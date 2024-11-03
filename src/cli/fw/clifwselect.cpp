/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#include "cli/fw/clifwselect.h"

#include <stdexcept>
#include "model/firewall.h"


namespace cli {
	CliFwSelectCommand::CliFwSelectCommand(CliContext& context) :
		CliCommand(context, 1, 1, new CliCommandFlags())
	{
	}


	void CliFwSelectCommand::do_execute(CliArgs& args, const CliCtrlcGuard& ctrlc_guard)
	{
		Firewall* firewall = context.get_firewall(args.pop());
		if (!firewall)
			throw std::runtime_error("firewall not found");

		context.set_current_firewall(firewall);
		context.logger->info("firewall '%s' selected", firewall->name().c_str());
	}

}
