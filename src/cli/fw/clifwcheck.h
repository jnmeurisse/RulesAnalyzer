/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#pragma once

#include "cli/clicmd.h"
#include "cli/clicmdmap.h"
#include "cli/clicontext.h"

namespace cli {

	class CliFwCheckCommand : public CliCommandMap
	{
	public:
		CliFwCheckCommand(CliContext& context);
	};


	class CliFwCheckAnyCommand : public CliCommand
	{
	public:
		CliFwCheckAnyCommand(CliContext& context);

	protected:
		virtual void do_execute(CliArgs& args, const CliCtrlcGuard& ctrlc_guard) override;
	};


	class CliFwCheckDenyCommand : public CliCommand
	{
	public:
		CliFwCheckDenyCommand(CliContext& context);

	protected:
		virtual void do_execute(CliArgs& args, const CliCtrlcGuard& ctrlc_guard) override;
	};


	class CliFwCheckAnomalyCommand : public CliCommand
	{
	public:
		CliFwCheckAnomalyCommand(CliContext& context);

		virtual void do_execute(CliArgs& args, const CliCtrlcGuard& ctrlc_guard) override;
	};


	class CliFwCheckSymmetryCommand : public CliCommand
	{
	public:
		CliFwCheckSymmetryCommand(CliContext& context);

	protected:
		virtual void do_execute(CliArgs& args, const CliCtrlcGuard& ctrlc_guard) override;
	};


	class CliFwCheckEquivalenceCommand : public CliCommand
	{
	public:
		CliFwCheckEquivalenceCommand(CliContext& context);

	protected:
		virtual void do_execute(CliArgs& args, const CliCtrlcGuard& ctrlc_guard) override;
	};


	class CliFwCheckAddressCommand : public CliCommand
	{
	public:
		CliFwCheckAddressCommand(CliContext& context);

		virtual void do_execute(CliArgs& args, const CliCtrlcGuard& ctrlc_guard) override;
	};


	class CliFwCheckServiceCommand : public CliCommand
	{
	public:
		CliFwCheckServiceCommand(CliContext& context);

		virtual void do_execute(CliArgs& args, const CliCtrlcGuard& ctrlc_guard) override;
	};


	class CliFwCheckAppCommand : public CliCommand
	{
	public:
		CliFwCheckAppCommand(CliContext& context);

	protected:
		virtual void do_execute(CliArgs& args, const CliCtrlcGuard& ctrlc_guard) override;
	};


	class CliFwCheckPacketCommand : public CliCommand
	{
	public:
		CliFwCheckPacketCommand(CliContext& context);

	protected:
		virtual void do_execute(CliArgs& args, const CliCtrlcGuard& ctrlc_guard) override;
	};

}
