/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#include "clifwshow.h"

#include <cassert>
#include <iostream>
#include <iterator>
#include <list>
#include <string>

#include "model/firewall.h"
#include "model/rule.h"
#include "model/table.h"
#include "model/tablewriter.h"
#include "tools/match.h"


namespace cli {

	CliFwShowCommand::CliFwShowCommand(CliContext& context) :
		CliCommandMap(context)
	{
		add("zones", new CliFwShowZonesCommand(context));
		add("rule",  new CliFwShowRuleCommand(context));
		add("rules", new CliFwShowRulesCommand(context));
		add(CommandKeys{ "address",     "addr" }, new CliFwShowAddressesCommand(context));
		add(CommandKeys{ "service",     "svc" },  new CliFwShowServicesCommand(context));
		add(CommandKeys{ "application", "app" },  new CliFwShowApplicationsCommand(context));
		add(CommandKeys{ "user" ,       "usr" },  new CliFwShowUsersCommand(context));
	}


	CliFwShowZonesCommand::CliFwShowZonesCommand(CliContext & context) :
		CliCommand(context, 0, 0, new CliCommandFlags({ CliCommandFlag::OutputToFile }))
	{
	}


	void CliFwShowZonesCommand::do_execute(CliArgs &args, const CliCtrlcGuard& ctrlc_guard)
	{
		const Firewall* firewall = context.fw;
		if (!firewall)
			report_firewall_not_selected();

		const Acl& acl = firewall->acl();
		if (acl.empty()) {
			context.logger->warning("firewall acl is empty");

		}
		else {
			std::cout << "    Zones (count of allow/deny rules)" << std::endl;
			std::cout << "    =================================" << std::endl;

			// Get all source zone names, make sure "any" is at the end.
			ZoneNames src_zone_names = acl.src_zone_names().move_at_end("any");
			ZoneNames dst_zone_names = acl.dst_zone_names().move_at_end("any");

			// Initialize and fill the table
			Table::Headers headers{ "zones" };
			std::copy(dst_zone_names.begin(), dst_zone_names.end(), std::back_inserter(headers));
			Table zones_table(headers);

			auto exclude_any = [](const Rule& rule) -> bool {return !rule.predicate().dst_zones().is_any(); };
			auto only_any = [](const Rule& rule) -> bool {return rule.predicate().dst_zones().is_any(); };

			for (const std::string& src_zone_name : src_zone_names) {
				// Get the next source zone
				const SrcZone* src_zone = context.nw.get_src_zone(src_zone_name);

				// Append a new row for this source zone
				Row *row = zones_table.add_row();

				// Add as many columns as there are destination zones.
				int column = 0;
				row->cell(column++).append(src_zone->name());
				for (const std::string& dst_zone_name : dst_zone_names) {
					const DstZone* dst_zone = context.nw.get_dst_zone(dst_zone_name);
	
					RuleList rules;
					if (dst_zone->is_any()) {
						rules = acl.filter(only_any).filter(*src_zone, *dst_zone);
					}
					else {
						rules = acl.filter(exclude_any).filter(*src_zone, *dst_zone);
					}

					// Add the number of allowed and denied rules.
					const RuleList::Counters counters = rules.get_counters();
					row->cell(column++).append(counters.to_string());
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
	}


	CliFwShowRulesCommand::CliFwShowRulesCommand(CliContext& context) :
		CliCommand(context, 0, 0, new CliCommandFlags({
										CliCommandFlag::OutputToFile,
										CliCommandFlag::ZoneFilter }))
	{
	}


	void CliFwShowRulesCommand::do_execute(CliArgs& args, const CliCtrlcGuard& ctrlc_guard)
	{
		const Firewall* firewall = context.fw;
		if (!firewall)
			report_firewall_not_selected();

		const RuleList rules = firewall->acl().filter_if(get_zones(args));
		const Table rules_table = rules.output(firewall->make_output_options(false));

		if (args.has_option(CliCommandFlag::OutputToFile)) {
			const std::string& output_file = args.output_file();
			if (write_table(output_file, rules_table, ctrlc_guard))
				context.logger->info("%zy rules written to '%s'",
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

	static std::string relation_message(int rule1, fwm::MnodeRelationship relation, int rule2)
	{
		switch (relation)
		{
		case fwm::MnodeRelationship::equal:
			return string_format("%d and %d are equal", rule1, rule2);
		case fwm::MnodeRelationship::subset:
			return string_format("%d is subset of %d", rule1, rule2);
		case fwm::MnodeRelationship::superset:
			return string_format("%d is superset of %d", rule1, rule2);
		case fwm::MnodeRelationship::disjoint:
			return string_format("%d and %d are disjoint", rule1, rule2);
		case fwm::MnodeRelationship::overlap:
			return string_format("%d overlaps with %d", rule1, rule2);
		default:
			return "n/a";
		}
	}


	void CliFwShowRuleCommand::do_execute(CliArgs& args, const CliCtrlcGuard& ctrlc_guard)
	{
		const Firewall* firewall = context.fw;
		if (!firewall)
			report_firewall_not_selected();

		int rule_id[2] = { -1, -1 };

		if (!str2i(args.pop(), rule_id[0]) || rule_id[0] < 0)
			report_invalid_rule_id();

		if (args.size() > 0) {
			if (!str2i(args.pop(), rule_id[1]) || rule_id[1] < 0)
				report_invalid_rule_id();
		}

		const Rule* rule[2] = { nullptr, nullptr };
		if (rule_id[0] >= 0) {
			rule[0] = firewall->get_rule(rule_id[0]);
			if (rule[0] == nullptr)
				report_rule_id_not_found(rule_id[0]);
		}

		if (rule_id[1] >= 0) {
			rule[1] = firewall->get_rule(rule_id[1]);
			if (rule[1] == nullptr)
				report_rule_id_not_found(rule_id[1]);
		}

		const RuleOutputOptions options = firewall->make_output_options(true);

		if (rule[0]) {
			Table rules_table{ rule[0]->output(options) };
			if (rule[1]) {
				const Predicate& predicate_0{ rule[0]->predicate() };
				const Predicate& predicate_1{ rule[1]->predicate() };

				// Initialize a table with two columns
				Table comparison_table{ {"", "comparison"} };

				comparison_table.add_row()->cell(1).append(
					relation_message(rule_id[0], predicate_0.compare(predicate_1), rule_id[1])
				);

				comparison_table.add_row()->cell(1).append(
					rule[0]->status() == rule[1]->status() ? "equal" : "different"
				);
				comparison_table.add_row()->cell(1).append(
					rule[0]->action() == rule[1]->action() ? "equal" : "different"
				);
				comparison_table.add_row()->cell(1).append(
					to_string(predicate_0.src_zones().compare(predicate_1.src_zones()))
				);
				comparison_table.add_row()->cell(1).append(
					to_string(predicate_0.dst_zones().compare(predicate_1.dst_zones()))
				);
				comparison_table.add_row()->cell(1).append(
					to_string(
						predicate_0
							.src_addresses()
							.negate_if(predicate_0.negate_src_addresses()
							).compare(
								predicate_1
								.src_addresses()
								.negate_if(predicate_1.negate_src_addresses())
							)
					)
				);
				comparison_table.add_row()->cell(1).append(
					to_string(
						predicate_0.
						dst_addresses()
							.negate_if(predicate_0.negate_dst_addresses()
							).compare(
								predicate_1
								.dst_addresses()
								.negate_if(predicate_1.negate_dst_addresses())
							)
					)
				);
				comparison_table.add_row()->cell(1).append(
					to_string(predicate_0.services().compare(predicate_1.services()))
				);

				if (options.contains(RuleOutputOption::APPLICATION_NAME)) {
					comparison_table.add_row()->cell(1).append(
						to_string(predicate_0.applications().compare(predicate_1.applications()))
					);
				}
				if (options.contains(RuleOutputOption::USER_NAME)) {
					comparison_table.add_row()->cell(1).append(
						to_string(predicate_0.users().compare(predicate_1.users()))
					);
				}

				rules_table
					.merge(rule[1]->output(options))
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
	};


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
		const Firewall* firewall = context.fw;
		if (!firewall)
			report_firewall_not_selected();

		const std::string wild_card = args.size() == 0 ? "*" : args.pop();

		// Get the rule list
		const RuleList rules = firewall->acl().filter_if(get_zones(args));

		// get referenced addresses and sort the list
		sortable_list<Ipv4Address> addresses{ rules.addresses()->items().sort_by_name() };

		Table addresses_table(Table::Headers{ "name", "address", "src rule id", "dst rule id" }, { 0, 20, 20, 20 });
		for (const Ipv4Address* address: addresses) {
			if (match(wild_card, address->name())) {
				Row* row = addresses_table.add_row();
				row->cell(0).append(address->name());
				row->cell(1).append(address->to_string());

				const SrcAddress* src_address{ context.nw.get_src_address(address->name()) };
				if (src_address) {
					row->cell(2).append(rules.filter(src_address).id_list());
				}

				const DstAddress* dst_address{ context.nw.get_dst_address(address->name()) };
				if (dst_address) {
					row->cell(3).append(rules.filter(dst_address).id_list());
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


	static bool compare_services(const Service& s1, const Service& s2)
	{
		if (s1.is_any())
			return false;
		else if (s1.protocol().lower() < s2.protocol().lower())
			return false;
		else if (s1.protocol().equal(s2.protocol()))
			return s1.compare_name(s2) == -1;
		else
			return true;
	}
	

	void CliFwShowServicesCommand::do_execute(CliArgs& args, const CliCtrlcGuard& ctrlc_guard)
	{
		const Firewall* firewall = context.fw;
		if (!firewall)
			report_firewall_not_selected();

		const std::string wild_card = args.size() == 0 ? "*" : args.pop();

		// Get the rule list
		const RuleList rules = firewall->acl().filter_if(get_zones(args));

		// get referenced services and sort the list by service type and then by name
		sortable_list<Service> services{ rules.services()->items() };
		services.sort(
			[](const Service* service1, const Service* service2) -> bool {
				return compare_services(*service1, *service2);
			}
		);

		Table services_table(Table::Headers{ "svc name", "protoport", "rule id" }, { 0, 20, 20 });
		for (const Service* service : services) {
			if (match(wild_card, service->name())) {
				Row* row = services_table.add_row();
				row->cell(0).append(service->name());
				row->cell(1).append(service->to_string());
				row->cell(2).append(rules.filter(service).id_list());
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
		const Firewall* firewall = context.fw;
		if (!firewall)
			report_firewall_not_selected();

		const std::string wild_card = args.size() == 0 ? "*" : args.pop();

		// Get the rule list
		const RuleList rules = firewall->acl().filter_if(get_zones(args));

		// get referenced applications and sort the list
		sortable_list<Application> applications{ rules.applications()->items().sort_by_name() };

		Table applications_table(Table::Headers{ "app name", "protoport", "rule id" }, { 0, 20, 20 });
		for (const Application* application : applications) {
			if (match(wild_card, application->name())) {
				Row* row = applications_table.add_row();
				row->cell(0).append(application->name());
				row->cell(1).append(application->services().names());
				row->cell(2).append(rules.filter(application).id_list());
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
		const Firewall* firewall = context.fw;
		if (!firewall)
			report_firewall_not_selected();

		const std::string wild_card = args.size() == 0 ? "*" : args.pop();

		// Get the rule list
		const RuleList rules = firewall->acl().filter_if(get_zones(args));

		// get referenced users and sort the list
		sortable_list<User> users{ rules.users()->items().sort_by_name() };

		Table users_table(Table::Headers{ "user name", "rule id" }, { 0, 20 });
		for (const User* user : users) {
			if (match(wild_card, user->name())) {
				Row* row = users_table.add_row();
				row->cell(0).append(user->name());
				row->cell(1).append(rules.filter(user).id_list());
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

}
