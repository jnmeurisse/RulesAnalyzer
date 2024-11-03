/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#include "cliosclear.h"

namespace cli {

	CliOsClearCommand::CliOsClearCommand(CliContext& context) :
		CliCommand(context, 0, 0, new CliCommandFlags())
	{
	}


	void CliOsClearCommand::do_execute(CliArgs& args, const CliCtrlcGuard& ctrlc_guard)
	{
		context.ostore.clear();
		context.logger->info("object store cleared");
	}

}
