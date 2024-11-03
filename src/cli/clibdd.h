/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#pragma once
#include "global.h"

#include "cli/clicmdmap.h"
#include "cli/clicontext.h"

namespace cli {

	class CliBddCommand : public CliCommandMap
	{
	public:
		CliBddCommand(CliContext& context);
	};


	class CliBddInfoCommand : public CliCommand
	{
	public:
		CliBddInfoCommand(CliContext& context);

		virtual void do_execute(CliArgs& args, const CliCtrlcGuard& ctrlc_guard) override;
	};


	class CliBddGcCommand : public CliCommand
	{
	public:
		CliBddGcCommand(CliContext& context);

		virtual void do_execute(CliArgs& args, const CliCtrlcGuard& ctrlc_guard) override;
	};
}
