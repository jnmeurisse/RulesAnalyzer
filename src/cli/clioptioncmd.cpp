/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#include "clioptioncmd.h"
#include "tools/strutil.h"

namespace cli {
	
	CliOptionCommand::CliOptionCommand(CliContext& context) :
		CliCommandMap(context)
	{
		add("enable", new CliEnableCommand(context));
		add("disable", new CliDisableCommand(context));
	}


	CliEnableCommand::CliEnableCommand(CliContext& context) :
		CliCommand(context, 1, 1, new CliCommandFlags())
	{
	}


	void CliEnableCommand::do_execute(CliArgs & args, const CliCtrlcGuard & ctrlc_guard)
	{
		if (iequal(args.front(), "app") || iequal(args.front(), "application"))
			context.nw.model_options.add(ModelOption::Application);
		else if (iequal(args.front(), "usr") || iequal(args.front(), "user"))
			context.nw.model_options.add(ModelOption::User);
		else
			report_unknown_model_option(args.front());
	}


	CliDisableCommand::CliDisableCommand(CliContext& context):
		CliCommand(context, 1, 1, new CliCommandFlags())
	{
	}


	void CliDisableCommand::do_execute(CliArgs & args, const CliCtrlcGuard & ctrlc_guard)
	{
		if (iequal(args.front(), "app") || iequal(args.front(), "application"))
			context.nw.model_options.remove(ModelOption::Application);
		else if (iequal(args.front(), "usr") || iequal(args.front(), "user"))
			context.nw.model_options.remove(ModelOption::User);
		else
			report_unknown_model_option(args.front());
	}

}
