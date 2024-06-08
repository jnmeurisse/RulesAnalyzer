/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#include "clifwcreate.h"

#include <memory>
#include <string>

#include "model/firewall.h"
#include "ostore/firewallfactory.h"


namespace cli {

	CliFwCreateCommand::CliFwCreateCommand(CliContext& context) :
		CliCommand(context, 1, 1, new CliCommandFlags())
	{
	};


	void CliFwCreateCommand::do_execute(CliArgs& args, const CliCtrlcGuard& ctrlc_guard)
	{
		const std::string firewall_name{ args.pop() };

		if (context.nw.get(firewall_name)) {
			context.logger->error("firewall '%s' already exist", firewall_name.c_str());

		}
		else {
			// create a new firewall
			FirewallPtr firewall = std::make_unique<Firewall>(
				firewall_name,
				context.nw
			);

			// add it to the network.
			context.nw.add(firewall);

			// make it current
			context.fw = context.nw.get(firewall_name);
			context.logger->info("firewall '%s' created", firewall_name.c_str());
		}
	};

}
