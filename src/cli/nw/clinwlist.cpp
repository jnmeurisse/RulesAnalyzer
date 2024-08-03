/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#include "clinwlist.h"

#include "model/network.h"

namespace cli {

	CliNwListCommand::CliNwListCommand(CliContext& context):
		CliCommand(context, 0, 0, new CliCommandFlags())
	{
	}

	void CliNwListCommand::do_execute(CliArgs& args, const CliCtrlcGuard& ctrlc_guard)
	{
		write_table(context.nw.info(), ctrlc_guard);
	}
}
