/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#pragma once
#include "global.h"

#include <string>

#include "cli/cliargs.h"
#include "cli/clicmdmap.h"
#include "cli/clicontext.h"

namespace cli {

	class CliOptionCommand : public CliCommandMap
	{
	public:
		explicit CliOptionCommand(CliContext &context);
	};


	class CliEnableCommand : public CliCommand
	{
	public:
		explicit CliEnableCommand(CliContext& context);

	protected:
		virtual void do_execute(CliArgs& args, const CliCtrlcGuard& ctrlc_guard) override;
	};


	class CliDisableCommand : public CliCommand
	{
	public:
		explicit CliDisableCommand(CliContext& context);

	protected:
		virtual void do_execute(CliArgs& args, const CliCtrlcGuard& ctrlc_guard) override;
	};

}
