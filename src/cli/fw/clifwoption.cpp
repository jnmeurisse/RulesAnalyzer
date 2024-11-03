/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#include "cli/fw/clifwoption.h"

#include "model/firewall.h"
#include "model/moptions.h"
#include "model/rule.h"
#include "tools/strutil.h"


namespace cli {
	CliFwEnableCommand::CliFwEnableCommand(CliContext& context) :
		CliCommandMap(context)
	{
		add("rule",  new CliFwEnableRuleCommand(context));
		add("model", new CliFwEnableModelCommand(context));
	}


	CliFwDisableCommand::CliFwDisableCommand(CliContext& context) :
		CliCommandMap(context)
	{
		add("rule",  new CliFwDisableRuleCommand(context));
		add("model", new CliFwDisableModelCommand(context));
	}


	CliFwEnableRuleCommand::CliFwEnableRuleCommand(CliContext & context) :
		CliCommand(context, 1, 1, new CliCommandFlags())
	{
	}


	void CliFwEnableRuleCommand::do_execute(CliArgs & args, const CliCtrlcGuard & ctrlc_guard)
	{
		Firewall& firewall = context.get_current_firewall();

		int rule_id = -1;

		if (!rat::str2i(args.pop(), rule_id) || rule_id < 0)
			report_invalid_rule_id();

		Rule* rule = firewall.get_rule(rule_id);
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
		Firewall& firewall = context.get_current_firewall();

		int rule_id = -1;

		if (!rat::str2i(args.pop(), rule_id) || rule_id < 0)
			report_invalid_rule_id();

		Rule* rule = firewall.get_rule(rule_id);
		if (rule == nullptr)
			report_rule_id_not_found(rule_id);

		rule->set_rule_status(RuleStatus::DISABLED);
		context.logger->info("rule id '%d' disabled", rule_id);
	}


	CliFwEnableModelCommand::CliFwEnableModelCommand(CliContext& context) :
		CliCommand(context, 1, 1, new CliCommandFlags())
	{
	}


	void CliFwEnableModelCommand::do_execute(CliArgs & args, const CliCtrlcGuard & ctrlc_guard)
	{
		Firewall& firewall = context.get_current_firewall();

		if (rat::iequal(args.front(), "app") || rat::iequal(args.front(), "application"))
			firewall.network().model_options.add(ModelOption::Application);
		else if (rat::iequal(args.front(), "usr") || rat::iequal(args.front(), "user"))
			firewall.network().model_options.add(ModelOption::User);
		else if (rat::iequal(args.front(), "url"))
			firewall.network().model_options.add(ModelOption::Url);
		else
			report_unknown_model_option(args.front());
	}


	CliFwDisableModelCommand::CliFwDisableModelCommand(CliContext& context):
		CliCommand(context, 1, 1, new CliCommandFlags())
	{
	}


	void CliFwDisableModelCommand::do_execute(CliArgs & args, const CliCtrlcGuard & ctrlc_guard)
	{
		Firewall& firewall = context.get_current_firewall();

		if (rat::iequal(args.front(), "app") || rat::iequal(args.front(), "application"))
			firewall.network().model_options.remove(ModelOption::Application);
		else if (rat::iequal(args.front(), "usr") || rat::iequal(args.front(), "user"))
			firewall.network().model_options.remove(ModelOption::User);
		else if (rat::iequal(args.front(), "url"))
			firewall.network().model_options.remove(ModelOption::Url);
		else
			report_unknown_model_option(args.front());
	}
}
