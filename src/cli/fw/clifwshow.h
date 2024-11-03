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


namespace cli {

	class CliFwShowCommand : public CliCommandMap
	{
	public:
		explicit CliFwShowCommand(CliContext& context);

	protected:
	};


	class CliFwShowZonesCommand : public CliCommand
	{
	public:
		explicit CliFwShowZonesCommand(CliContext& context);

	protected:
		virtual void do_execute(CliArgs& args, const CliCtrlcGuard& ctrlc_guard) override;
	};


	class CliFwShowRuleCommand : public CliCommand
	{
	public:
		explicit CliFwShowRuleCommand(CliContext& context);

	protected:
		virtual void do_execute(CliArgs& args, const CliCtrlcGuard& ctrlc_guard) override;

	private:
		void show_comparison_result(const std::string& item_name, MnodeRelationship relation);
	};


	class CliFwShowRulesCommand : public CliCommand
	{
	public:
		explicit CliFwShowRulesCommand(CliContext& context);

	protected:
		virtual void do_execute(CliArgs& args, const CliCtrlcGuard& ctrlc_guard) override;
	};



	class CliFwShowAddressesCommand : public CliCommand
	{
	public:
		explicit CliFwShowAddressesCommand(CliContext& context);

	protected:
		virtual void do_execute(CliArgs& args, const CliCtrlcGuard& ctrlc_guard) override;
	};


	class CliFwShowServicesCommand : public CliCommand
	{
	public:
		explicit CliFwShowServicesCommand(CliContext& context);

	protected:
		virtual void do_execute(CliArgs& args, const CliCtrlcGuard& ctrlc_guard) override;
	};


	class CliFwShowApplicationsCommand : public CliCommand
	{
	public:
		explicit CliFwShowApplicationsCommand(CliContext& context);

	protected:
		virtual void do_execute(CliArgs& args, const CliCtrlcGuard& ctrlc_guard) override;
	};


	class CliFwShowUsersCommand : public CliCommand
	{
	public:
		explicit CliFwShowUsersCommand(CliContext& context);

	protected:
		virtual void do_execute(CliArgs& args, const CliCtrlcGuard& ctrlc_guard) override;
	};


	class CliFwShowUrlsCommand : public CliCommand
	{
	public:
		explicit CliFwShowUrlsCommand(CliContext& context);

	protected:
		virtual void do_execute(CliArgs& args, const CliCtrlcGuard& ctrlc_guard) override;
	};

}
