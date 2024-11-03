/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#include "cli/fw/clifwinfo.h"


namespace cli {

	CliFwInfoCommand::CliFwInfoCommand(CliContext& context) :
		CliCommand(context, 0, 0, new CliCommandFlags())
	{
	}


	void CliFwInfoCommand::do_execute(CliArgs& args, const CliCtrlcGuard& ctrlc_guard)
	{
		write_table(
			context.get_current_firewall().info(),
			ctrlc_guard
		);
	}

}
