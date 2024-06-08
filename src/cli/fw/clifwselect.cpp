/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#include "clifwselect.h"

#include "model/firewall.h"

namespace cli {
	CliFwSelectCommand::CliFwSelectCommand(CliContext& context) :
		CliCommand(context, 1, 1, new CliCommandFlags())
	{
	};


	void CliFwSelectCommand::do_execute(CliArgs& args, const CliCtrlcGuard& ctrlc_guard)
	{
		const std::string firewall_name{ args.pop() };

		Firewall* firewall = context.nw.get(firewall_name);
		if (!firewall)
			report_firewall_not_selected();

		context.fw = firewall;
		context.logger->info("firewall '%s' selected", firewall_name.c_str());
	};

}
