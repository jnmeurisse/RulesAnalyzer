/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#include "cli/fw/clifwshow.h"

#include <iostream>
#include <iterator>
#include <list>
#include <string>

#include "model/address.h"
#include "model/firewall.h"
#include "model/rule.h"
#include "model/table.h"
#include "model/tablewriter.h"
#include "tools/match.h"


namespace cli {
	using namespace fwm;


	CliFwShowCommand::CliFwShowCommand(CliContext& context) :
		CliCommandMap(context)
	{
		add("zones",                              new CliFwShowZonesCommand(context));
		add("rule",                               new CliFwShowRuleCommand(context));
		add("rules",                              new CliFwShowRulesCommand(context));
		add(CommandKeys{ "address",     "addr" }, new CliFwShowAddressesCommand(context));
		add(CommandKeys{ "service",     "svc" },  new CliFwShowServicesCommand(context));
		add(CommandKeys{ "application", "app" },  new CliFwShowApplicationsCommand(context));
		add(CommandKeys{ "user" ,       "usr" },  new CliFwShowUsersCommand(context));
		add("url",                                new CliFwShowUrlsCommand(context));
	}


	CliFwShowZonesCommand::CliFwShowZonesCommand(CliContext & context) :
		CliCommand(context, 0, 0, new CliCommandFlags({ CliCommandFlag::OutputToFile }))
	{
	}


	void CliFwShowZonesCommand::do_execute(CliArgs &args, const CliCtrlcGuard& ctrlc_guard)
	{
		const Firewall& firewall = context.get_current_firewall();

		const RuleList acl = firewall.acl();
		if (acl.size() == 0) {
			context.logger->warning("firewall acl is empty");
			return;
		}

		std::cout << "    Zones (count of allow/deny rules)" << std::endl;
		std::cout << "    =================================" << std::endl;

		// Get all source and destination zones sorted by name and move "any" at the end of the list.
		const SrcZone* any_src_zone = context.get_any_src_zone();
		const DstZone* any_dst_zone = context.get_any_dst_zone();

		SrcZoneListPtr all_src_zones = acl.all_src_zones({any_src_zone});
		DstZoneListPtr all_dst_zones = acl.all_dst_zones({any_dst_zone});
		all_src_zones->push_back(any_src_zone);
		all_dst_zones->push_back(any_dst_zone);

		// Initialize and fill the table
		Table zones_table(
			Table::create_headers(
				"zones",
				all_dst_zones->names()
			)
		);

		auto exclude_any = [](const Rule& rule) -> bool {return !rule.predicate().dst_zones().is_any(); };
		auto only_any = [](const Rule& rule) -> bool {return rule.predicate().dst_zones().is_any(); };

		for (const SrcZone* src_zone : *all_src_zones) {
			// Append a new row for this source zone
			Row& row = zones_table.add_row();

			// Add as many columns as there are destination zones.
			int column = 0;
			row.cell(column++).append(src_zone->name());
			for (const DstZone* dst_zone : *all_dst_zones) {
				RuleList rules;

				if (dst_zone->is_any()) {
					rules = acl.filter(only_any).filter(*src_zone, *dst_zone);
				}
				else {
					rules = acl.filter(exclude_any).filter(*src_zone, *dst_zone);
				}

				// Add the number of allowed and denied rules.
				const RuleList::Counters counters = rules.get_counters();
				row.cell(column++).append(counters.to_string());
			}
		}

		if (args.has_option(CliCommandFlag::OutputToFile)) {
			const std::string& output_file{ args.output_file() };
			if (write_table(output_file, zones_table, ctrlc_guard))
				context.logger->info("%zu zones written to '%s'",
					zones_table.row_count(),
					output_file.c_str());
		}
		else {
			write_table(zones_table, ctrlc_guard);
		}
	}


	CliFwShowRulesCommand::CliFwShowRulesCommand(CliContext& context) :
		CliCommand(context, 0, 0, new CliCommandFlags({
										CliCommandFlag::OutputToFile,
										CliCommandFlag::ZoneFilter }))
	{
	}


	void CliFwShowRulesCommand::do_execute(CliArgs& args, const CliCtrlcGuard& ctrlc_guard)
	{
		const Firewall& firewall = context.get_current_firewall();

		const RuleList acl = firewall.acl();
		if (acl.size() == 0) {
			context.logger->warning("firewall acl is empty");
			return;
		}

		// get the zones filter from the command line
		ZonePairOptArg zones_filter = get_zones_filter(args);

		// get the rules filtered using the optional zones filter
		const RuleList rules = zones_filter ? acl.filter(zones_filter.value()) : acl;

		// create a table showing the selected rules
		const Table rules_table = rules.create_table(firewall.make_output_options(false));

		if (args.has_option(CliCommandFlag::OutputToFile)) {
			const std::string& output_file = args.output_file();

			if (write_table(output_file, rules_table, ctrlc_guard))
				context.logger->info("%zu rules written to '%s'",
					rules_table.row_count(),
					output_file.c_str());
		}
		else {
			write_table(rules_table, ctrlc_guard);
		}
	}


	CliFwShowRuleCommand::CliFwShowRuleCommand(CliContext& context) :
		CliCommand(context, 1, 2, new CliCommandFlags({CliCommandFlag::OutputToFile}))
	{
	}

	static std::string relation_message(int rule1, MnodeRelationship relation, int rule2)
	{
		switch (relation)
		{
		case MnodeRelationship::equal:
			return fmt::format("{} and {} are equal", rule1, rule2);
		case MnodeRelationship::subset:
			return fmt::format("{} is subset of {}", rule1, rule2);
		case MnodeRelationship::superset:
			return fmt::format("{} is superset of {}", rule1, rule2);
		case MnodeRelationship::disjoint:
			return fmt::format("{} and {} are disjoint", rule1, rule2);
		case MnodeRelationship::overlap:
			return fmt::format("{} overlaps with {}", rule1, rule2);
		default:
			return "n/a";
		}
	}


	void CliFwShowRuleCommand::do_execute(CliArgs& args, const CliCtrlcGuard& ctrlc_guard)
	{
		const Firewall& firewall = context.get_current_firewall();

		const RuleList acl = firewall.acl();
		if (acl.size() == 0) {
			context.logger->warning("firewall acl is empty");
			return;
		}

		int rule_id[2] = { -1, -1 };

		if (!rat::str2i(args.pop(), rule_id[0]) || rule_id[0] < 0)
			report_invalid_rule_id();

		if (args.size() > 0) {
			if (!rat::str2i(args.pop(), rule_id[1]) || rule_id[1] < 0)
				report_invalid_rule_id();
		}

		const Rule* rule[2] = { nullptr, nullptr };
		const Predicate* predicate[2] = {nullptr, nullptr};

		if (rule_id[0] >= 0) {
			rule[0] = firewall.get_rule(rule_id[0]);
			if (rule[0] == nullptr)
				report_rule_id_not_found(rule_id[0]);
			predicate[0] = &rule[0]->predicate();
		}

		if (rule_id[1] >= 0) {
			rule[1] = firewall.get_rule(rule_id[1]);
			if (rule[1] == nullptr)
				report_rule_id_not_found(rule_id[1]);
			predicate[1] = &rule[1]->predicate();
		}

		const RuleOutputOptions options = firewall.make_output_options(true);

		if (rule[0]) {
			Table rules_table{ rule[0]->create_table(options) };
			if (rule[1]) {
				// Initialize a table with two columns
				Table comparison_table{ {"", "comparison"} };

				comparison_table.add_row().cell(1).append(
					relation_message(rule_id[0], rule[0]->compare(*rule[1]), rule_id[1])
				);

				comparison_table.add_row().cell(1).append(
					rule[0]->status() == rule[1]->status() ? "equal" : "different"
				);
				comparison_table.add_row().cell(1).append(
					rule[0]->action() == rule[1]->action() ? "equal" : "different"
				);
				comparison_table.add_row().cell(1).append(
					to_string(predicate[0]->src_zones().compare(predicate[1]->src_zones()))
				);
				comparison_table.add_row().cell(1).append(
					to_string(predicate[0]->dst_zones().compare(predicate[1]->dst_zones()))
				);
				comparison_table.add_row().cell(1).append(
					to_string(
						Bddnode(predicate[0]->src_addresses_bdd()).compare(predicate[1]->src_addresses())
					)
				);
				comparison_table.add_row().cell(1).append(
					to_string(
						Bddnode(predicate[0]->dst_addresses_bdd()).compare(predicate[1]->dst_addresses())
					)
				);
				comparison_table.add_row().cell(1).append(
					to_string(predicate[0]->services().compare(predicate[1]->services()))
				);

				if (options.contains(RuleOutputOption::APPLICATION_NAME)) {
					comparison_table.add_row().cell(1).append(
						to_string(predicate[0]->applications().compare(predicate[1]->applications()))
					);
				}

				if (options.contains(RuleOutputOption::USER_NAME)) {
					comparison_table.add_row().cell(1).append(
						to_string(predicate[0]->users().compare(predicate[1]->users()))
					);
				}

				if (options.contains(RuleOutputOption::URL)) {
					comparison_table.add_row().cell(1).append(
						to_string(predicate[0]->urls().compare(predicate[1]->urls()))
					);
				}

				rules_table
					.merge(rule[1]->create_table(options))
					.merge(comparison_table);
			}

			if (args.has_option(CliCommandFlag::OutputToFile)) {
				const std::string& output_file = args.output_file();

				if (write_table(output_file, rules_table, ctrlc_guard))
					context.logger->info("%zu rules written to '%s'",
						rules_table.row_count(),
						output_file.c_str());
			}
			else {
				write_table(rules_table, ctrlc_guard);
			}
		}
	}


	void CliFwShowRuleCommand::show_comparison_result(const std::string& item_name, MnodeRelationship relation)
	{
		std::cout
			<< std::string(4, ' ')
			<< item_name
			<< " "
			<< to_string(relation)
			<< std::endl;
	}


	CliFwShowAddressesCommand::CliFwShowAddressesCommand(CliContext & context) :
		CliCommand(context, 0, 1, new CliCommandFlags({
										CliCommandFlag::OutputToFile,
										CliCommandFlag::ZoneFilter }))
	{
	}


	void CliFwShowAddressesCommand::do_execute(CliArgs& args, const CliCtrlcGuard& ctrlc_guard)
	{
		const Firewall& firewall = context.get_current_firewall();

		const RuleList acl = firewall.acl();
		if (acl.size() == 0) {
			context.logger->warning("firewall acl is empty");
			return;
		}

		// get the wildcard from the command line, if missing use * as a default value
		const std::string wild_card = args.size() == 0 ? "*" : args.pop();

		// get the zones filter from the command line
		ZonePairOptArg zones_filter = get_zones_filter(args);

		// get the rules filtered using the optional zones filter
		const RuleList rules = zones_filter ? acl.filter(zones_filter.value()) : acl;

		// get all referenced addresses
		AddressListPtr all_addresses = rules.all_addresses();

		// Output all addresses sorted by name
		Table addresses_table(Table::Headers{ "addr name", "address", "src rule id", "dst rule id" }, { 0, 20, 20, 20 });

		for (const IpAddress* address: all_addresses->sort()) {
			if (rat::match(wild_card, address->name())) {
				Row& row = addresses_table.add_row();

				row.cell(0).append(address->name());
				row.cell(1).append(address->to_string());

				const SrcAddress* src_address{ context.network.get_src_address(address->name()) };
				if (src_address) {
					row.cell(2).append(rules.filter(src_address).id_list());
				}

				const DstAddress* dst_address{ context.network.get_dst_address(address->name()) };
				if (dst_address) {
					row.cell(3).append(rules.filter(dst_address).id_list());
				}
			}
		}

		if (args.has_option(CliCommandFlag::OutputToFile)) {
			const std::string& output_file = args.output_file();

			if (write_table(output_file, addresses_table, ctrlc_guard))
				context.logger->info("%zu addresses written to '%s'",
					addresses_table.row_count(),
					output_file.c_str());
		}
		else {
			write_table(addresses_table, ctrlc_guard);
		}
	}


	CliFwShowServicesCommand::CliFwShowServicesCommand(CliContext& context) :
		CliCommand(context, 0, 1, new CliCommandFlags({
									CliCommandFlag::OutputToFile,
									CliCommandFlag::ZoneFilter }))
	{
	}


	void CliFwShowServicesCommand::do_execute(CliArgs& args, const CliCtrlcGuard& ctrlc_guard)
	{
		const Firewall& firewall = context.get_current_firewall();

		const RuleList acl = firewall.acl();
		if (acl.size() == 0) {
			context.logger->warning("firewall acl is empty");
			return;
		}

		// get the wildcard from the command line, if missing use * as a default value
		const std::string wild_card = args.size() == 0 ? "*" : args.pop();

		// get the zones filter from the command line
		ZonePairOptArg zones_filter = get_zones_filter(args);

		// get the rules filtered using the optional zones filter
		const RuleList rules = zones_filter ? acl.filter(zones_filter.value()) : acl;

		// get all referenced addresses
		AddressListPtr all_addresses = rules.all_addresses();

		// get referenced services
		ServiceListPtr all_services = rules.all_services();

		// Output all services sorted by service type and then by name
		Table services_table(Table::Headers{ "svc name", "protoport", "rule id" }, { 0, 20, 20 });
		for (const Service* service : all_services->sort()) {
			if (rat::match(wild_card, service->name())) {
				Row& row = services_table.add_row();

				row.cell(0).append(service->name());
				row.cell(1).append(service->to_string());
				row.cell(2).append(rules.filter(service).id_list());
			}
		}

		if (args.has_option(CliCommandFlag::OutputToFile)) {
			const std::string& output_file = args.output_file();

			if (write_table(output_file, services_table, ctrlc_guard))
				context.logger->info("%zu services written to '%s'",
					services_table.row_count(),
					output_file.c_str());
		}
		else {
			write_table(services_table, ctrlc_guard);
		}
	}


	CliFwShowApplicationsCommand::CliFwShowApplicationsCommand(CliContext& context) :
		CliCommand(context, 0, 1, new CliCommandFlags({
										CliCommandFlag::OutputToFile,
										CliCommandFlag::ZoneFilter }))
	{
	}


	void CliFwShowApplicationsCommand::do_execute(CliArgs& args, const CliCtrlcGuard & ctrlc_guard)
	{
		const Firewall& firewall = context.get_current_firewall();

		const RuleList acl = firewall.acl();
		if (acl.size() == 0) {
			context.logger->warning("firewall acl is empty");
			return;
		}

		// get the wildcard from the command line, if missing use * as a default value
		const std::string wild_card = args.size() == 0 ? "*" : args.pop();

		// get the zones filter from the command line
		ZonePairOptArg zones_filter = get_zones_filter(args);

		// get the rules filtered using the optional zones filter
		const RuleList rules = zones_filter ? acl.filter(zones_filter.value()) : acl;

		// get referenced applications sorted by name
		ApplicationListPtr all_applications = rules.all_applications();

		// Output all applications
		Table applications_table{ Table::Headers{ "app name", "protoport", "rule id" }, { 0, 20, 20 } };

		for (const Application* application : all_applications->sort()) {
			if (rat::match(wild_card, application->name())) {
				Row& row = applications_table.add_row();

				row.cell(0).append(application->name());
				row.cell(1).append(application->services().names());
				row.cell(2).append(rules.filter(application).id_list());
			}
		}

		if (args.has_option(CliCommandFlag::OutputToFile)) {
			const std::string& output_file = args.output_file();

			if (write_table(output_file, applications_table, ctrlc_guard))
				context.logger->info("%zu applications written to '%s'",
					applications_table.col_count(),
					output_file.c_str());
		}
		else {
			write_table(applications_table, ctrlc_guard);
		}
	}


	CliFwShowUsersCommand::CliFwShowUsersCommand(CliContext& context) :
		CliCommand(context, 0, 1, new CliCommandFlags({
										CliCommandFlag::OutputToFile,
										CliCommandFlag::ZoneFilter }))
	{
	}


	void CliFwShowUsersCommand::do_execute(CliArgs& args, const CliCtrlcGuard & ctrlc_guard)
	{
		const Firewall& firewall = context.get_current_firewall();

		const RuleList acl = firewall.acl();
		if (acl.size() == 0) {
			context.logger->warning("firewall acl is empty");
			return;
		}

		// get the wildcard from the command line, if missing use * as a default value
		const std::string wild_card = args.size() == 0 ? "*" : args.pop();

		// get the zones filter from the command line
		ZonePairOptArg zones_filter = get_zones_filter(args);

		// get the rules filtered using the optional zones filter
		const RuleList rules = zones_filter ? acl.filter(zones_filter.value()) : acl;

		// get referenced users
		UserListPtr all_users = rules.all_users();

		// Output all users sorted by name
		Table users_table(Table::Headers{ "user name", "rule id" }, { 0, 20 });
		for (const User* user : all_users->sort()) {
			if (rat::match(wild_card, user->name())) {
				Row& row = users_table.add_row();

				row.cell(0).append(user->name());
				row.cell(1).append(rules.filter(user).id_list());
			}
		}

		if (args.has_option(CliCommandFlag::OutputToFile)) {
			const std::string& output_file = args.output_file();

			if (write_table(output_file, users_table, ctrlc_guard))
				context.logger->info("%zu users written to '%s'",
					users_table.row_count(),
					output_file.c_str());
		}
		else {
			write_table(users_table, ctrlc_guard);
		}
	}


	CliFwShowUrlsCommand::CliFwShowUrlsCommand(CliContext& context) :
		CliCommand(context, 0, 1, new CliCommandFlags({
										CliCommandFlag::OutputToFile,
										CliCommandFlag::ZoneFilter }))
	{
	}

	void CliFwShowUrlsCommand::do_execute(CliArgs& args, const CliCtrlcGuard & ctrlc_guard)
	{
		const Firewall& firewall = context.get_current_firewall();

		const RuleList acl = firewall.acl();
		if (acl.size() == 0) {
			context.logger->warning("firewall acl is empty");
			return;
		}

		// get the wildcard from the command line, if missing use * as a default value
		const std::string wild_card = args.size() == 0 ? "*" : args.pop();

		// get the zones filter from the command line
		ZonePairOptArg zones_filter = get_zones_filter(args);

		// get the rules filtered using the optional zones filter
		const RuleList rules = zones_filter ? acl.filter(zones_filter.value()) : acl;

		// get referenced users
		UrlListPtr all_urls = rules.all_urls();

		// Output all url sorted by name
		Table urls_table(Table::Headers{ "url", "rule id" }, { 0, 20 });
		for (const Url* url : all_urls->sort()) {
			if (rat::match(wild_card, url->name())) {
				Row& row = urls_table.add_row();

				row.cell(0).append(url->name());
				row.cell(1).append(rules.filter(url).id_list());
			}
		}

		if (args.has_option(CliCommandFlag::OutputToFile)) {
			const std::string& output_file = args.output_file();

			if (write_table(output_file, urls_table, ctrlc_guard))
				context.logger->info("%zu urls written to '%s'",
					urls_table.row_count(),
					output_file.c_str());
		}
		else {
			write_table(urls_table, ctrlc_guard);
		}
	}

}
