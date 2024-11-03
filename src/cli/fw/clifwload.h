/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#pragma once
#include "global.h"

#include <list>
#include <string>

#include "cli/cliargs.h"
#include "cli/clicmd.h"
#include "cli/clicontext.h"


namespace cli {

	class CliFwLoadCommand : public CliCommand
	{
	public:
		CliFwLoadCommand(CliContext& context);

		virtual void do_execute(CliArgs& args, const CliCtrlcGuard& ctrlc_guard) override;

	private:
		void show_unresolved(std::list<std::string>& unresolved, const std::string& name, const std::string& plural_suffix);
	};

}
