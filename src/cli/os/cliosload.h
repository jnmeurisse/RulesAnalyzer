/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#pragma once

#include <string>

#include "cli/clicmd.h"
#include "cli/clicmdmap.h"
#include "cli/clicontext.h"
#include "tools/interrupt.h"

namespace cli {
	class CliOsLoadCommand : public CliCommandMap
	{
	public:
		CliOsLoadCommand(CliContext& context);
	};


	class CliOsLoadSubCommand : public CliCommand
	{
	public:
		CliOsLoadSubCommand(CliContext& context);

		virtual void do_execute(CliArgs& args, const CliCtrlcGuard& ctrlc_guard) override;

	private:
		virtual int do_load_file(const std::string& filename, f_interrupt_cb interrupt_cb) = 0;
	};


	class CliLoadAdrCommand : public CliOsLoadSubCommand
	{
	public:
		using CliOsLoadSubCommand::CliOsLoadSubCommand;

	private:
		virtual int do_load_file(const std::string& filename, f_interrupt_cb interrupt_cb) override
		{
			return context.os.load_addresses(filename, interrupt_cb);
		}
	};


	class CliLoadAdrgCommand : public CliOsLoadSubCommand
	{
	public:
		using CliOsLoadSubCommand::CliOsLoadSubCommand;

	private:
		virtual int do_load_file(const std::string& filename, f_interrupt_cb interrupt_cb) override
		{
			return context.os.load_address_groups(filename, interrupt_cb);
		}
	};


	class CliLoadSvcCommand : public CliOsLoadSubCommand
	{
	public:
		using CliOsLoadSubCommand::CliOsLoadSubCommand;

	private:
		virtual int do_load_file(const std::string& filename, f_interrupt_cb interrupt_cb) override
		{
			return context.os.load_services(filename, interrupt_cb);
		}
	};


	class CliLoadSvcgCommand : public CliOsLoadSubCommand
	{
	public:
		using CliOsLoadSubCommand::CliOsLoadSubCommand;

	private:
		virtual int do_load_file(const std::string& filename, f_interrupt_cb interrupt_cb) override
		{
			return context.os.load_service_groups(filename, interrupt_cb);
		}
	};


	class CliLoadAppCommand : public CliOsLoadSubCommand
	{
	public:
		using CliOsLoadSubCommand::CliOsLoadSubCommand;

	private:
		virtual int do_load_file(const std::string& filename, f_interrupt_cb interrupt_cb) override
		{
			return context.os.load_apps(filename, interrupt_cb);
		}
	};


	class CliLoadAppgCommand : public CliOsLoadSubCommand
	{
	public:
		using CliOsLoadSubCommand::CliOsLoadSubCommand;

	private:
		virtual int do_load_file(const std::string& filename, f_interrupt_cb interrupt_cb) override
		{
			return context.os.load_app_groups(filename, interrupt_cb);
		}
	};


	class CliLoadUsrCommand : public CliOsLoadSubCommand
	{
	public:
		using CliOsLoadSubCommand::CliOsLoadSubCommand;

	private:
		virtual int do_load_file(const std::string& filename, f_interrupt_cb interrupt_cb) override
		{
			return context.os.load_users(filename, interrupt_cb);
		}
	};


	class CliLoadUsrgCommand : public CliOsLoadSubCommand
	{
	public:
		using CliOsLoadSubCommand::CliOsLoadSubCommand;

	private:
		virtual int do_load_file(const std::string& filename, f_interrupt_cb interrupt_cb) override
		{
			return context.os.load_user_groups(filename, interrupt_cb);
		}
	};

}
