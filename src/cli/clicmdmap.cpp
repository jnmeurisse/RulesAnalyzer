/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#include "clicmdmap.h"


namespace cli {

	void CliCommandMap::add(const std::string& key, CliCommand* command)
	{
		_commands[key].reset(command);
	}


	void CliCommandMap::add(const CommandKeys& keys, CliCommand* command)
	{
		std::shared_ptr<CliCommand> tmp{ command };

		for (const std::string& key : keys)
			_commands[key] = tmp;
	}


	void CliCommandMap::do_execute(CliArgs& args, const CliCtrlcGuard& ctrlc_guard)
	{
		_commands.at(args.pop()).get()->execute(args, ctrlc_guard);
	}
}
