/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#include "clifwinfo.h"

namespace cli {

	CliFwInfoCommand::CliFwInfoCommand(CliContext& context) :
		CliCommand(context, 0, 0, new CliCommandFlags())
	{
	}


	void CliFwInfoCommand::do_execute(CliArgs& args, const CliCtrlcGuard& ctrlc_guard)
	{
		const Firewall* firewall = context.fw;
		if (!firewall)
			report_firewall_not_selected();

		write_table(firewall->info(), ctrlc_guard);
	}

}
