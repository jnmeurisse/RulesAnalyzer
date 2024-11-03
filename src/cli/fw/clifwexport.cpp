/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#include "cli/fw/clifwexport.h"

#include <string>
#include "model/firewall.h"
#include "model/rule.h"
#include "model/table.h"


namespace cli {

	CliFwExportCommand::CliFwExportCommand(CliContext& context) :
		CliCommand(context, 1, 1, new CliCommandFlags())
	{
	}


	void CliFwExportCommand::do_execute(CliArgs& args, const CliCtrlcGuard& ctrlc_guard)
	{
		const Firewall& firewall = context.get_current_firewall();

		const RuleList acl = firewall.acl();
		if (acl.size() == 0) {
			context.logger->warning("firewall acl is empty");
			return;
		}

		const RuleOutputOptions options = firewall.make_output_options(true);
		const Table rules_table{ firewall.create_rules_table(options) };
		const std::string output_file{ args.pop() };

		if (write_table(output_file, rules_table, ctrlc_guard)) {
			context.logger->info("rules exported to file '%s'", output_file.c_str());
		}
	}

}
