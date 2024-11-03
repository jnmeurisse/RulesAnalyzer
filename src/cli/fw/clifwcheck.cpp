/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#include "cli/fw/clifwcheck.h"

#include <algorithm>
#include <chrono>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <string>
#include <vector>

#include "model/analyzer.h"
#include "model/anomaly.h"
#include "model/comparator.h"
#include "model/firewall.h"
#include "model/packettester.h"
#include "model/rule.h"
#include "model/rulelist.h"
#include "model/service.h"
#include "model/table.h"
#include "model/tablewriter.h"
#include "model/zone.h"
#include "tools/strutil.h"
#include "fmt/core.h"


namespace cli {

	CliFwCheckCommand::CliFwCheckCommand(CliContext& context) :
		CliCommandMap(context)
	{
		add("any",                               new CliFwCheckAnyCommand(context));
		add("deny",                              new CliFwCheckDenyCommand(context));
		add("anomaly",                           new CliFwCheckAnomalyCommand(context));
		add("symmetry",                          new CliFwCheckSymmetryCommand(context));
		add("equivalence",                       new CliFwCheckEquivalenceCommand(context));
		add(CommandKeys{ "addr", "address" },    new CliFwCheckAddressCommand(context));
		add(CommandKeys{ "svc",  "service" },    new CliFwCheckServiceCommand(context));
		add(CommandKeys{ "app", "application" }, new CliFwCheckAppCommand(context));
		add("packet",                            new CliFwCheckPacketCommand(context));
	}


	CliFwCheckAnyCommand::CliFwCheckAnyCommand(CliContext& context) :
		CliCommand(context, 0, 1, new CliCommandFlags({
										CliCommandFlag::OutputToFile,
										CliCommandFlag::ZoneFilter}))
	{
	}


	void CliFwCheckAnyCommand::do_execute(CliArgs& args, const CliCtrlcGuard& ctrlc_guard)
	{
		const Firewall& firewall = context.get_current_firewall();

		const RuleList acl = firewall.acl();
		if (acl.size() == 0) {
			context.logger->warning("firewall acl is empty");
			return;
		}

		// define what is an "any" destination address
		DstAddressArgs destinations;
		if (args.size() == 0) {
			// this is the definition when there is no parameter on the command line.
			if (context.network.config().ip_model == IPAddressModel::IP64Model) {
				destinations.add(context.network.get_dst_address("any4"));
				destinations.add(context.network.get_dst_address("any6"));
			}
			else {
				destinations.add(context.network.get_dst_address("any"));
			}
		}
		else {
			// this is the definition on the command line.
			destinations = get_destination_addresses(args);
		}

		// get the zones filter from the command line
		ZonePairOptArg zones_filter = get_zones_filter(args);

		// get the rules filtered using the optional zones filter
		const RuleList filtered_rules = zones_filter ? acl.filter(zones_filter.value()) : acl;

		// allocate the analyzer
		const Analyzer analyzer{ filtered_rules, context.network.config().ip_model };

		// search for any/any rules
		const RuleList any_any_rules{ analyzer.check_any(destinations.list()) };

		if (any_any_rules.size() == 0) {
			context.logger->info("any/any rule not found");

		}
		else {
			context.logger->info("%zu any/any %s found",
				any_any_rules.size(),
				rat::pluralize(any_any_rules.size(), "rule").c_str());

			Table rules_table{ {"rule id", "rule name", "src.zone", "src.addr", "dst.zone"} };
			for (const Rule* rule : any_any_rules) {
				const Predicate& predicate = rule->predicate();

				Row& row = rules_table.add_row();

				row.cell(0).append(rule->id());
				row.cell(1).append(rule->name());
				predicate.src_zones().write_to_cell(row.cell(2), MnodeInfoType::NAME);
				predicate.src_addresses().write_to_cell(row.cell(3), MnodeInfoType::NAME);
				predicate.dst_zones().write_to_cell(row.cell(4), MnodeInfoType::NAME);
			}

			if (args.has_option(CliCommandFlag::OutputToFile)) {
				const std::string& output_file{ args.output_file() };

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


	CliFwCheckDenyCommand::CliFwCheckDenyCommand(CliContext& context) :
		CliCommand(context, 0, 0, new CliCommandFlags())
	{
	}


	void CliFwCheckDenyCommand::do_execute(CliArgs& args, const CliCtrlcGuard& ctrlc_guard)
	{
		const Firewall& firewall = context.get_current_firewall();

		const RuleList acl = firewall.acl();
		if (acl.size() == 0) {
			context.logger->warning("firewall acl is empty");
			return;
		}

		// allocate the analyzer
		Analyzer analyzer{ acl, context.network.config().ip_model };

		// search for deny rules
		RuleList deny_rules = analyzer.check_deny();
		if (deny_rules.size() == 0) {
			context.logger->info("deny all rule not found");
		}
		else if (deny_rules.size() > 1) {
			context.logger->warning("multiple deny all rules found");
		}
		else {
			context.logger->info("deny all found at rule id %d",
				deny_rules.id_list()[0]
			);
		}
	}


	CliFwCheckAnomalyCommand::CliFwCheckAnomalyCommand(CliContext& context) :
		CliCommand(context, 0, 0, new CliCommandFlags({
										CliCommandFlag::OutputToFile,
										CliCommandFlag::ZoneFilter }))
	{
	}


	void CliFwCheckAnomalyCommand::do_execute(CliArgs& args, const CliCtrlcGuard& ctrlc_guard)
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
		const RuleList filtered_rules = zones_filter ? acl.filter(zones_filter.value()) : acl;

		// allocate the analyzer
		const Analyzer analyzer{ filtered_rules, context.network.config().ip_model };

		// search for anomalies
		const auto start_time = std::chrono::steady_clock::now();
		const RuleAnomalies anomalies = analyzer.check_anomaly(ctrlc_guard.get_interrupt_cb());

		if (anomalies.empty()) {
			context.logger->info("no anomalies found");

		}
		else {
			// show elapsed time if it is significative
			const auto end_time = std::chrono::steady_clock::now();
			const auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
			if (elapsed.count() > 100)
				context.logger->info(
					"%zu rules processed in %.3f seconds",
					analyzer.acl().size(),
					elapsed.count() * 1.0e-3f
				);

			if (anomalies.missing_deny_all)
				context.logger->warning("a deny all rule is missing");

			const Table anomalies_table = anomalies.output_anomalies(acl.have_names());

			if (args.has_option(CliCommandFlag::OutputToFile)) {
				const std::string& output_file = args.output_file();

				if (write_table(output_file, anomalies_table, ctrlc_guard))
					context.logger->info("%zu anomalies written to '%s'",
						anomalies_table.row_count(),
						output_file.c_str()
					);
			}
			else {
				write_table(anomalies_table, ctrlc_guard);
			}
		}
	}


	CliFwCheckSymmetryCommand::CliFwCheckSymmetryCommand(CliContext& context) :
		CliCommand(context, 0, 0, new CliCommandFlags({
										CliCommandFlag::OutputToFile,
										CliCommandFlag::ZoneFilter,
										CliCommandFlag::ZoneFilter }))
	{
	}


	void CliFwCheckSymmetryCommand::do_execute(CliArgs& args, const CliCtrlcGuard& ctrlc_guard)
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
		const RuleList filtered_rules = zones_filter ? acl.filter(zones_filter.value()) : acl;

		// allocate the analyzer
		const Analyzer analyzer{ filtered_rules, context.network.config().ip_model };

		// search for symmetrical rules.
		const std::list<RulePair> symmetrical_rules = analyzer.check_symmetry(true, ctrlc_guard.get_interrupt_cb());

		if (symmetrical_rules.empty()) {
			context.logger->info("no symmetrical rules found");
		}
		else
		{
			Table rules_table;
			const RuleOutputOptions options = firewall.make_output_options(true);

			for (const RulePair& rule_pair : symmetrical_rules) {
				Table rule_pair_table = std::get<0>(rule_pair)->create_table(options);
				rule_pair_table.merge(std::get<1>(rule_pair)->create_table(options));

				rules_table.append(rule_pair_table);
			}

			if (args.has_option(CliCommandFlag::OutputToFile)) {
				const std::string& output_file{ args.output_file() };

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


	CliFwCheckEquivalenceCommand::CliFwCheckEquivalenceCommand(CliContext& context) :
		CliCommand(context, 1, 1, new CliCommandFlags({ CliCommandFlag::ZoneFilter }))
	{
	}


	void CliFwCheckEquivalenceCommand::do_execute(CliArgs& args, const CliCtrlcGuard& ctrlc_guard)
	{
		const Firewall& firewall1 = context.get_current_firewall();

		// get the zones filter from the command line
		ZonePairOptArg zones_filter = get_zones_filter(args);

		// get the rules filtered using the optional zones filter
		const RuleList rule_list1 = zones_filter.has_value()
										? firewall1.acl().filter(zones_filter.value())
										: firewall1.acl();

		const std::string firewall_name = args.pop();
		const Firewall* firewall2 = context.get_firewall(firewall_name);
		if (!firewall2)
			throw std::runtime_error(fmt::format("firewall '{}' not found", firewall_name));

		// get the rules from the second firewall filtered using the optional zones filter
		const RuleList rule_list2 = zones_filter.has_value()
										? firewall2->acl().filter(zones_filter.value())
										: firewall2->acl();

		// run the policy comparator
		const PolicylistRelationShip relation = PolicyListComparator::compare(rule_list1, rule_list2);

		// output the comparison results
		if (relation.allowed == MnodeRelationship::equal && relation.denied == MnodeRelationship::equal) {
			context.logger->info("rules are equivalent");

		}
		else {
			context.logger->warning("rules are NOT equivalent");
			context.logger->warning(" allowed traffic : %s", to_string(relation.allowed).c_str());
			context.logger->warning(" denied traffic  : %s", to_string(relation.denied).c_str());
		}
	}


	const RuleList CliFwCheckCommonCommand::get_zone_filtered_rules(CliArgs& args, const RuleList& acl) const
	{
		// get the zones filter from the command line
		ZonePairOptArg zones_filter = get_zones_filter(args);

		// get the rules filtered using the optional zones filter
		return zones_filter ? acl.filter(zones_filter.value()) : acl;
	}


	Table CliFwCheckCommonCommand::create_zone_summary(const RuleList& acl, const RuleList& filtered_rules) const
	{
		// The "any" source and destination zones are excluded from the table.
		ZoneListPtr all_zones = acl.all_zones(
			{
				context.get_any_src_zone(),
				context.get_any_dst_zone()
			}
		);

		Table zones_table = Table(
			Table::create_headers(
				"zones",
				all_zones->names()
			)
		);

		SrcZoneListPtr all_src_zones = acl.all_src_zones({context.get_any_src_zone()});
		DstZoneListPtr all_dst_zones = acl.all_dst_zones({context.get_any_dst_zone()});

		for (const SrcZone* src_zone : *all_src_zones) {
			Row& row = zones_table.add_row();
			int col = 0;

			// add a new row for this source zone
			row.cell(col++).append(src_zone->name());

			// for each destination zone, show the number of rules filtered
			for (const DstZone* dst_zone : *all_dst_zones) {
				const RuleList::Counters counters = filtered_rules
					.filter(*src_zone, *dst_zone)
					.get_counters();

				if (counters.allowed > 0)
					row.cell(col).append(counters.allowed);

				col++;
			}
		}

		return zones_table;
	}



	CliFwCheckAddressCommand::CliFwCheckAddressCommand(CliContext& context) :
		CliFwCheckCommonCommand(context, 1, 1, new CliCommandFlags({
										CliCommandFlag::OutputToFile,
										CliCommandFlag::IncludeAny,
										CliCommandFlag::ZoneFilter }))
	{
	}


	void CliFwCheckAddressCommand::do_execute(CliArgs& args, const CliCtrlcGuard& ctrlc_guard)
	{
		const Firewall& firewall = context.get_current_firewall();

		const RuleList acl = firewall.acl();
		if (acl.size() == 0) {
			context.logger->warning("firewall acl is empty");
			return;
		}

		// get the addresses from the command line
		AddressArgs addresses = get_addresses_arg(args);

		// get the rules filtered using the optional zones filter
		const RuleList zone_filtered_rules = get_zone_filtered_rules(args, acl);

		// keep rules allowing the specified addresses
		bool include_any = args.has_option(CliCommandFlag::IncludeAny)
			|| addresses.src_addr_args.list().is_any()
			|| addresses.dst_addr_args.list().is_any();

		const RuleList address_filtered_rules{
			// select Allow rules
			zone_filtered_rules.filter(RuleAction::ALLOW)
			// filter rules that allow the specified addresses either in source or destination
			.filter(
				[&addresses, include_any](const Rule& rule) -> bool {
					const Predicate& predicate = rule.predicate();
					const SrcAddressGroup& src_addresses = predicate.src_addresses();
					const DstAddressGroup& dst_addresses = predicate.dst_addresses();
					const bool is_src_any = src_addresses.is_any();
					const bool is_dst_any = dst_addresses.is_any();
					return
						(addresses.src_addr_args.list().size() > 0 && (
							(include_any && is_src_any) ||
							(!include_any && !is_src_any
								&& addresses.src_addr_args.list().is_subset(src_addresses.negate_if(predicate.negate_src_addresses())))
							)
						)
						||
						(addresses.dst_addr_args.list().size() > 0 && (
							(include_any && is_dst_any) ||
							(!include_any && !is_dst_any
								&& addresses.dst_addr_args.list().is_subset(dst_addresses.negate_if(predicate.negate_dst_addresses())))
							)
						);
				})
		};

		// create a table showing all rules referencing the address as a source address
		// or destination address.
		const Table rules_table = address_filtered_rules.create_table(firewall.make_output_options(false));

		if (args.has_option(CliCommandFlag::OutputToFile)) {
			const std::string& output_file{ args.output_file() };

			if (write_table(output_file, rules_table, ctrlc_guard))
				context.logger->info("%zu rules written to '%s'",
					rules_table.row_count(),
					output_file.c_str());
		}
		else {
			// create a table showing a summary of the usage of the given addresses between zones.
			const Table summary_table = create_zone_summary(acl, address_filtered_rules);
			write_table(summary_table, ctrlc_guard);

			std::cout << std::endl;
			write_table(rules_table, ctrlc_guard);
		}

	}


	CliFwCheckServiceCommand::CliFwCheckServiceCommand(CliContext& context) :
		CliFwCheckCommonCommand(context, 1, 1, new CliCommandFlags({
										CliCommandFlag::OutputToFile,
										CliCommandFlag::IncludeAny,
										CliCommandFlag::ZoneFilter }))
	{
	}


	void CliFwCheckServiceCommand::do_execute(CliArgs& args, const CliCtrlcGuard& ctrlc_guard)
	{
		const Firewall& firewall = context.get_current_firewall();

		const RuleList acl = firewall.acl();
		if (acl.size() == 0) {
			context.logger->warning("firewall acl is empty");
			return;
		}

		// get the services from the command line
		ServiceArgs services = get_services_arg(args);
		if (services.list().empty())
			throw std::runtime_error("service not specified");

		// get the rules filtered using the optional zones filter
		const RuleList zone_filtered_rules = get_zone_filtered_rules(args, acl);

		const bool include_any = args.has_option(CliCommandFlag::IncludeAny) || services.list().is_any();

		const RuleList service_filtered_rules{
			// select Allow rules
			zone_filtered_rules.filter(RuleAction::ALLOW)
			// filter rules that allow the specified services
			.filter(services.list())
			// Remove rules that allow only "any services" unless -any is on the command line
			// (-any is assumed if the filter criteria is "any services")
			.filter(
				[include_any](const Rule& rule) -> bool {
					return include_any ? true : !rule.predicate().services().is_any();
				}
			)
		};


		const Table rules_table = service_filtered_rules.create_table(firewall.make_output_options(false));

		if (args.has_option(CliCommandFlag::OutputToFile)) {
			const std::string& output_file{ args.output_file() };

			if (write_table(output_file, rules_table, ctrlc_guard))
				context.logger->info("rules written to '%s'",
					rules_table.row_count(),
					output_file.c_str());
		}
		else {
			// create a table showing a summary of the usage of the given services between zones.
			const Table summary_table = create_zone_summary(acl, service_filtered_rules);
			write_table(summary_table, ctrlc_guard);

			std::cout << std::endl;
			write_table(rules_table, ctrlc_guard);
		}
	}


	CliFwCheckAppCommand::CliFwCheckAppCommand(CliContext& context) :
		CliFwCheckCommonCommand(context, 1, 2, new CliCommandFlags({
										CliCommandFlag::OutputToFile,
										CliCommandFlag::IncludeAny,
										CliCommandFlag::ZoneFilter }))
	{
	}


	void CliFwCheckAppCommand::do_execute(CliArgs& args, const CliCtrlcGuard& ctrlc_guard)
	{
		const Firewall& firewall = context.get_current_firewall();

		const RuleList acl = firewall.acl();
		if (acl.size() == 0) {
			context.logger->warning("firewall acl is empty");
			return;
		}

		if (!context.network.model_options.contains(ModelOption::Application))
			throw std::runtime_error("application model is disabled");

		// get the application from the command line
		const ApplicationArgs applications{ get_applications_arg(args) };

		// get the services from the command line if specified
		const ServiceArgs services{ get_services_arg(args) };

		// get the rules filtered using the optional zones filter
		const RuleList zone_filtered_rules = get_zone_filtered_rules(args, acl);

		// Filter the rules by applications and optionally by services
		const bool include_any = args.has_option(CliCommandFlag::IncludeAny) || applications.list().is_any();
		const RuleList application_filtered_rules {
			// select Allow rules
			zone_filtered_rules.filter(RuleAction::ALLOW)
			// select rules by applications and optionally by services
			.filter(applications.list(), services.list())
			// Remove rules that allow only "any applications" unless -any is on the command line
			// (-any is assumed if the filter criteria is "any applications")
			.filter(
				[include_any](const Rule& rule) -> bool {
					return include_any? true : !rule.predicate().applications().is_any(); }
			)
		};

		Table rules_table = application_filtered_rules.create_table(firewall.make_output_options(false));

		if (args.has_option(CliCommandFlag::OutputToFile)) {
			const std::string& output_file{ args.output_file() };

			if (write_table(output_file, rules_table, ctrlc_guard))
				context.logger->info("rules written to '%s'",
					rules_table.row_count(),
					output_file.c_str());
		}
		else {
			// create a table showing a summary of the usage of the given application between zones.
			const Table summary_table = create_zone_summary(acl, application_filtered_rules);
			write_table(summary_table, ctrlc_guard);

			std::cout << std::endl;
			write_table(rules_table, ctrlc_guard);
		}

	}


	CliFwCheckPacketCommand::CliFwCheckPacketCommand(CliContext& context) :
		CliCommand(context, 5, 5, new CliCommandFlags())
	{
	}


	void CliFwCheckPacketCommand::do_execute(CliArgs& args, const CliCtrlcGuard& ctrlc_guard)
	{
		const Firewall& firewall = context.get_current_firewall();

		const RuleList acl = firewall.acl();
		if (acl.size() == 0) {
			context.logger->warning("firewall acl is empty");
			return;
		}

		// Get the source zone (optional, can be replaced by _ or an empty string)
		SrcZoneOptArg src_zone_arg = get_src_zone(args, true);
		std::unique_ptr<const SrcZone> src_zone = std::unique_ptr<const SrcZone>(src_zone_arg ? &src_zone_arg.value() : nullptr);

		// Decode the source addresses
		const SrcAddressArgs sources = get_source_addresses_arg(args);

		// Get the destination zone (optional, can be replaced by _ or an empty string)
		DstZoneOptArg dst_zone_arg = get_dst_zone(args, true);
		std::unique_ptr<const DstZone> dst_zone = std::unique_ptr<const DstZone>(dst_zone_arg ? &dst_zone_arg.value() : nullptr);

		// Decode the destination addresses
		const DstAddressArgs destinations = get_destination_addresses(args);

		// Decode the services
		const ServiceArgs services = get_services_arg(args);

		// Check if the packet is allowed
		fwm::PacketTester packet_tester{ acl };
		auto result = packet_tester.is_packet_allowed(
			src_zone.release(),
			sources.list(),
			dst_zone.release(),
			destinations.list(),
			services.list(),
			nullptr,
			nullptr,
			nullptr
		);

		if (!result.second)
			context.logger->info("packets are denied");

		else {
			context.logger->info(
				fmt::format("rule {} {} this packet",
					result.second->id(),
					(result.first ? "allows" : "denies")
				).c_str()
			);
		}

	}

}
