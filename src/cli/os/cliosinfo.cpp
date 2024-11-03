/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#include "cliosinfo.h"


namespace cli {

	CliOsInfoCommand::CliOsInfoCommand(CliContext& context) :
		CliCommand(context, 0, 0, new CliCommandFlags())
	{
	}


	void CliOsInfoCommand::do_execute(CliArgs& args, const CliCtrlcGuard& ctrlc_guard)
	{
		write_table(context.ostore.info(), ctrlc_guard);
	}

}
