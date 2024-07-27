/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#include "clifwexport.h"

#include <string>

#include "model/firewall.h"

namespace cli {

	CliFwExportCommand::CliFwExportCommand(CliContext& context) :
		CliCommand(context, 1, 1, new CliCommandFlags())
	{
	};


	void CliFwExportCommand::do_execute(CliArgs& args, const CliCtrlcGuard& ctrlc_guard)
	{
		const Firewall* firewall = context.fw;
		if (!firewall)
			report_firewall_not_selected();

		const RuleOutputOptions options = firewall->make_output_options(true);
		const Table rules_table{ firewall->output_rules(options) };
		const std::string output_file{ args.pop() };

		if (write_table(output_file, rules_table, ctrlc_guard)) {
			context.logger->info("rules exported to file '%s'", output_file.c_str());
		}
	};

}
