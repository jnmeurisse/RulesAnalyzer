/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#pragma once
#include "global.h"

#include <map>
#include <memory>
#include <string>

#include "cli/cliargs.h"
#include "cli/clicmd.h"


namespace cli {

	class CliCommandMap : public CliCommand {
	public:
		using CliCommand::CliCommand;
		using CommandKeys = std::list<std::string>;

		void add(const std::string& key, CliCommand* command);
		void add(const CommandKeys& keys, CliCommand* command);

	protected:
		virtual void do_execute(CliArgs& args, const CliCtrlcGuard& ctrlc_guard) override;

	private:
		std::map<std::string, std::shared_ptr<CliCommand>, rat::stri_comparator> _commands;
	};

}
