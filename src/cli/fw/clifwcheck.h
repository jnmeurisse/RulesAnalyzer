/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#pragma once
#include "global.h"

#include "cli/clicmd.h"
#include "cli/clicmdmap.h"
#include "cli/clicontext.h"
#include "tools/optional.h"


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


	class CliFwCheckCommonCommand : public CliCommand
	{
	protected:
		using CliCommand::CliCommand;

		/*
		 * returns the rules filtered using the optional zones filter
		*/
		const RuleList get_zone_filtered_rules(CliArgs& args, const RuleList& acl) const;

		/*
		 *
		*/
		Table create_zone_summary(const RuleList& acl, const RuleList& filtered_rules) const;
	};


	class CliFwCheckAddressCommand : public CliFwCheckCommonCommand
	{
	public:
		CliFwCheckAddressCommand(CliContext& context);

		virtual void do_execute(CliArgs& args, const CliCtrlcGuard& ctrlc_guard) override;
	};


	class CliFwCheckServiceCommand : public CliFwCheckCommonCommand
	{
	public:
		CliFwCheckServiceCommand(CliContext& context);

		virtual void do_execute(CliArgs& args, const CliCtrlcGuard& ctrlc_guard) override;
	};


	class CliFwCheckAppCommand : public CliFwCheckCommonCommand
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
