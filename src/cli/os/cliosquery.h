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

#include "cli/clicmd.h"
#include "cli/clicmdmap.h"
#include "cli/clicontext.h"

#include "model/table.h"

#include "ostore/addressobject.h"
#include "ostore/applicationobject.h"
#include "ostore/serviceobject.h"
#include "ostore/userobject.h"

namespace cli {

	class CliOsQueryCommand : public CliCommandMap
	{
	public:
		CliOsQueryCommand(CliContext& context);
	};


	class CliOsQuerySubCommand : public CliCommand
	{
	public:
		CliOsQuerySubCommand(CliContext& context);

		virtual void do_execute(CliArgs& args, const CliCtrlcGuard& ctrlc_guard) override;

	private:
		virtual fwm::Table do_query(const std::string& query) = 0;
	};


	class CliQueryAdrCommand : public CliOsQuerySubCommand
	{
	public:
		using CliOsQuerySubCommand::CliOsQuerySubCommand;

	private:
		virtual fwm::Table do_query(const std::string& query) override;
	};


	class CliQueryAdrgCommand : public CliOsQuerySubCommand
	{
	public:
		using CliOsQuerySubCommand::CliOsQuerySubCommand;

	private:
		virtual fwm::Table do_query(const std::string& query) override;
	};


	class CliQuerySvcCommand : public CliOsQuerySubCommand
	{
	public:
		using CliOsQuerySubCommand::CliOsQuerySubCommand;

	private:
		virtual fwm::Table do_query(const std::string& query) override;
	};


	class CliQuerySvcgCommand : public CliOsQuerySubCommand
	{
	public:
		using CliOsQuerySubCommand::CliOsQuerySubCommand;

	private:
		virtual fwm::Table do_query(const std::string& query) override;
	};


	class CliQueryAppCommand : public CliOsQuerySubCommand
	{
	public:
		using CliOsQuerySubCommand::CliOsQuerySubCommand;

	private:
		virtual fwm::Table do_query(const std::string& query) override;
	};


	class CliQueryAppgCommand : public CliOsQuerySubCommand
	{
	public:
		using CliOsQuerySubCommand::CliOsQuerySubCommand;

	private:
		virtual fwm::Table do_query(const std::string& query) override;
	};


	class CliQueryUsrCommand : public CliOsQuerySubCommand
	{
	public:
		using CliOsQuerySubCommand::CliOsQuerySubCommand;

	private:
		virtual fwm::Table do_query(const std::string& query) override;
	};


	class CliQueryUsrgCommand : public CliOsQuerySubCommand
	{
	public:
		using CliOsQuerySubCommand::CliOsQuerySubCommand;

	private:
		virtual fwm::Table do_query(const std::string& query) override;
	};


	class CliQueryUrlCommand : public CliOsQuerySubCommand
	{
	public:
		using CliOsQuerySubCommand::CliOsQuerySubCommand;

	private:
		virtual fwm::Table do_query(const std::string& query) override;
	};


	class CliQueryUrlgCommand : public CliOsQuerySubCommand
	{
	public:
		using CliOsQuerySubCommand::CliOsQuerySubCommand;

	private:
		virtual fwm::Table do_query(const std::string& query) override;
	};
}
