/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#pragma once
#include "global.h"

#include "cli/cliargs.h"
#include "cli/clicmdmap.h"
#include "cli/clicontext.h"


namespace cli {

	class CliFwEnableCommand : public CliCommandMap
	{
	public:
		CliFwEnableCommand(CliContext& context);
	};


	class CliFwDisableCommand : public CliCommandMap
	{
	public:
		explicit CliFwDisableCommand(CliContext& context);
	};


	class CliFwEnableRuleCommand : public CliCommand
	{
	public:
		CliFwEnableRuleCommand(CliContext& context);

	protected:
		virtual void do_execute(CliArgs& args, const CliCtrlcGuard& ctrlc_guard) override;
	};


	class CliFwDisableRuleCommand : public CliCommand
	{
	public:
		CliFwDisableRuleCommand(CliContext& context);

	protected:
		virtual void do_execute(CliArgs& args, const CliCtrlcGuard& ctrlc_guard) override;
	};

	class CliFwEnableModelCommand : public CliCommand
	{
	public:
		CliFwEnableModelCommand(CliContext& context);

	protected:
		virtual void do_execute(CliArgs& args, const CliCtrlcGuard& ctrlc_guard) override;
	};


	class CliFwDisableModelCommand : public CliCommand
	{
	public:
		CliFwDisableModelCommand(CliContext& context);

	protected:
		virtual void do_execute(CliArgs& args, const CliCtrlcGuard& ctrlc_guard) override;
	};

}
