/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#include "clifwoption.h"

namespace cli {
	CliFwEnableCommand::CliFwEnableCommand(CliContext& context) : 
		CliCommandMap(context)
	{
		add("rule", new CliFwEnableRuleCommand(context));
	}


	CliFwDisableCommand::CliFwDisableCommand(CliContext& context) :
		CliCommandMap(context)
	{
		add("rule", new CliFwDisableRuleCommand(context));
	}


	CliFwEnableRuleCommand::CliFwEnableRuleCommand(CliContext & context) :
		CliCommand(context, 1, 1, new CliCommandFlags())
	{
	}

	
	void CliFwEnableRuleCommand::do_execute(CliArgs & args, const CliCtrlcGuard & ctrlc_guard)
	{
		Firewall* firewall = context.fw;
		if (!firewall)
			report_firewall_not_selected();

		int rule_id = -1;

		if (!str2i(args.pop(), rule_id) || rule_id < 0)
			report_invalid_rule_id();

		Rule* rule = firewall->get_rule(rule_id);
		if (rule == nullptr)
			report_rule_id_not_found(rule_id);

		rule->set_rule_status(RuleStatus::ENABLED);
		context.logger->info("rule id '%d' enabled", rule_id);
	}
	
	
	CliFwDisableRuleCommand::CliFwDisableRuleCommand(CliContext & context) :
		CliCommand(context, 1, 1, new CliCommandFlags())
	{
	}

	
	void CliFwDisableRuleCommand::do_execute(CliArgs & args, const CliCtrlcGuard & ctrlc_guard)
	{
		Firewall* firewall = context.fw;
		if (!firewall)
			report_firewall_not_selected();

		int rule_id = -1;

		if (!str2i(args.pop(), rule_id) || rule_id < 0)
			report_invalid_rule_id();

		Rule* rule = firewall->get_rule(rule_id);
		if (rule == nullptr)
			report_rule_id_not_found(rule_id);

		rule->set_rule_status(RuleStatus::DISABLED);
		context.logger->info("rule id '%d' disabled", rule_id);
	}

}
