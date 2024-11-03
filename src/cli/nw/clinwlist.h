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

	class CliNwListCommand : public CliCommand
	{
	public:
		explicit CliNwListCommand(CliContext& context);

	protected:
		virtual void do_execute(CliArgs& args, const CliCtrlcGuard& ctrlc_guard) override;
	};

}
