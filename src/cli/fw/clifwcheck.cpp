/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#include "clifwcheck.h"

#include <algorithm>
#include <chrono>
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

#include "tools/strutil.h"


namespace cli {

	CliFwCheckCommand::CliFwCheckCommand(CliContext& context) :
		CliCommandMap(context)
	{
		add("any",      new CliFwCheckAnyCommand(context));
		add("deny",     new CliFwCheckDenyCommand(context));
		add("anomaly",  new CliFwCheckAnomalyCommand(context));
		add("symmetry", new CliFwCheckSymmetryCommand(context));
		add("equivalence", new CliFwCheckEquivalenceCommand(context));
		add(CommandKeys{ "addr", "address" },    new CliFwCheckAddressCommand(context));
		add(CommandKeys{ "svc",  "service" },    new CliFwCheckServiceCommand(context));
		add(CommandKeys{ "app", "application" }, new CliFwCheckAppCommand(context));
		add("packet", new CliFwCheckPacketCommand(context));
	}


	CliFwCheckAnyCommand::CliFwCheckAnyCommand(CliContext& context) :
		CliCommand(context, 0, 1, new CliCommandFlags({
										CliCommandFlag::OutputToFile,
										CliCommandFlag::ZoneFilter}))
	{
	}


	void CliFwCheckAnyCommand::do_execute(CliArgs& args, const CliCtrlcGuard& ctrlc_guard)
	{
		const Firewall* firewall = context.fw;
		if (!firewall)
			report_firewall_not_selected();

		DestinationAddressesArgs destinations;

		if (args.size() == 0) {
			destinations.list->add_member(context.nw.get_dst_address("any"));
		}
		else {
			destinations = get_destination_addresses(args);
		}

		const Analyzer analyzer{ firewall->acl().filter_if(get_zones(args)) };
		const RuleList rules{ analyzer.check_any(*destinations.list) };
		if (rules.empty()) {
			context.logger->info("no any/any rule found");
		}
		else {
			context.logger->info("%zu any/any %s found",
				rules.size(),
				pluralize(rules.size(), "rule"));

			Table rules_table{ {"rule id", "rule name", "src.zone", "src.addr", "dst.zone"} };
			for (const Rule* rule : rules) {
				Row* row = rules_table.add_row();

				row->cell(0).append(rule->id());
				row->cell(1).append(rule->name());
				const Predicate& predicate = rule->predicate();
				predicate.src_zones().output(row->cell(2), MnodeInfoType::NAME);
				predicate.src_addresses().output(row->cell(3), MnodeInfoType::NAME);
				predicate.dst_zones().output(row->cell(4), MnodeInfoType::NAME);
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
		const Firewall* firewall = context.fw;
		if (!firewall)
			report_firewall_not_selected();

		Analyzer analyzer{ firewall->acl() };
		RuleList rules{ analyzer.check_deny() };
		if (rules.empty()) {
			context.logger->info("no deny all rule found");
		}
		else if (rules.size() > 1) {
			context.logger->warning("multiple deny all rules found");
		}
		else {
			context.logger->info("deny all rule found, rule id = %d",
				rules.id_list()[0]
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
		const Firewall* firewall = context.fw;
		if (!firewall)
			report_firewall_not_selected();

		const auto begin = std::chrono::steady_clock::now();
		const Analyzer analyzer{ firewall->acl().filter_if(get_zones(args)) };
		const RuleAnomalies anomalies{ analyzer.check_anomaly(ctrlc_guard.get_interrupt_cb()) };

		if (anomalies.empty()) {
			context.logger->info("no anomalies found");
		}
		else {
			const auto end = std::chrono::steady_clock::now();
			const auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin);
			if (elapsed.count() > 100)
				context.logger->info(
					"%zu rules processed in %.3f seconds",
					analyzer.acl().size(), 
					elapsed.count() * 1.0e-3f
				);

			if (anomalies.missing_deny_all)
				context.logger->warning("a deny all rule is missing");

			const Table anomalies_table = anomalies.output_anomalies(firewall->acl().have_names());

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
		CliCommand(context, 0, 1, new CliCommandFlags({
										CliCommandFlag::OutputToFile,
										CliCommandFlag::ZoneFilter,
										CliCommandFlag::ZoneFilter }))
	{
	}


	void CliFwCheckSymmetryCommand::do_execute(CliArgs& args, const CliCtrlcGuard& ctrlc_guard)
	{
		const Firewall* firewall = context.fw;
		if (!firewall)
			report_firewall_not_selected();

		bool strict{ true };
		if (args.size() > 0) {
			const std::string strict_arg{ args.pop() };
			if (strict_arg == "=")
				strict = true;
			else if (strict_arg == "<")
				strict = false;
			else
				throw std::runtime_error("invalid parameter");
		}

		// Search for symmetrical rules.
		const Analyzer analyzer{ firewall->acl().filter_if(get_zones(args)) };
		const std::list<RulePair> symmetrical_rules = analyzer.check_symmetry(strict, ctrlc_guard.get_interrupt_cb());

		if (symmetrical_rules.empty()) {
			context.logger->info("no symmetrical rules found");
		}
		else
		{
			Table rules_table;
			const RuleOutputOptions options = firewall->make_output_options(true);

			for (const RulePair rule_pair : symmetrical_rules) {
				Table rule_pair_table = std::get<0>(rule_pair)->output(options);
				rule_pair_table.merge(std::get<1>(rule_pair)->output(options));

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
		ZonePair zones = get_zones(args);

		const Firewall* firewall1 = context.fw;
		if (!firewall1)
			report_firewall_not_selected();
		const RuleList list1 = firewall1->acl().filter_if(zones);

		const std::string firewall_name = args.pop();
		const Firewall* firewall2 = context.nw.get(firewall_name);
		if (!firewall2)
			throw std::runtime_error(string_format("firewall '%s' not found", firewall_name.c_str()));
		const RuleList list2 = firewall2->acl().filter_if(zones);

		const PolicylistRelationShip relation = PolicyListComparator::compare(list1, list2);
		if (relation.allowed == MnodeRelationship::equal && relation.denied == MnodeRelationship::equal) {
			std::cout << "Rules are equivalent." << std::endl;
		}
		else {
			std::cout << "Rules are NOT equivalent." << std::endl;
			std::cout << "Allowed traffic : " << to_string(relation.allowed) << std::endl;
			std::cout << "Denied traffic  : " << to_string(relation.denied) << std::endl;
		}
	}


	CliFwCheckAddressCommand::CliFwCheckAddressCommand(CliContext& context) :
		CliCommand(context, 1, 1, new CliCommandFlags({
										CliCommandFlag::OutputToFile,
										CliCommandFlag::IncludeAny,
										CliCommandFlag::ZoneFilter }))
	{
	}


	void CliFwCheckAddressCommand::do_execute(CliArgs& args, const CliCtrlcGuard& ctrlc_guard)
	{
		const Firewall* firewall = context.fw;
		if (!firewall)
			report_firewall_not_selected();

		// Decode the addresses
		AddressesArgs addresses = get_addresses_arg(args);

		// get all rules allowing the specified addresses
		bool include_any = args.has_option(CliCommandFlag::IncludeAny)
			|| addresses.src.list->is_any()
			|| addresses.dst.list->is_any();

		const RuleList rules{
			// select all rules or select rules filtered by zones
			firewall->acl().filter_if(get_zones(args))
			// keep only allow
			.filter(RuleAction::ALLOW)
			// filter rules that allow the specified addresses either in source or destination
			.filter(
				[&addresses, include_any](const Rule& rule) -> bool {
					const Predicate& predicate = rule.predicate();
					const SrcAddressGroup& src_addresses = predicate.src_addresses();
					const DstAddressGroup& dst_addresses = predicate.dst_addresses();
					const bool is_src_any = src_addresses.is_any();
					const bool is_dst_any = dst_addresses.is_any();
					return
						(addresses.src.list->size() > 0 &&
						(include_any && is_src_any) ||
							(!include_any && !is_src_any
								&& addresses.src.list->is_subset(src_addresses.negate_if(predicate.negate_src_addresses())))
							)
						||
						(addresses.dst.list->size() > 0 &&
						(include_any && is_dst_any) ||
							(!include_any && !is_dst_any
								&& addresses.dst.list->is_subset(dst_addresses.negate_if(predicate.negate_dst_addresses())))
							);
				})
		};

		// build the column headers, exclude any zone from the header
		const ZoneNames zone_names{ firewall->acl().zone_names() };
		Table::Headers headers{ "zones" };
		std::copy_if(
			zone_names.begin(),
			zone_names.end(),
			std::back_inserter(headers),
			[](const std::string& zone) {return !iequal(zone, "any"); }
		);

		Table zones_table{ headers };

		for (const std::string& zone_source : zone_names) {
			const SrcZone* src_zone = context.nw.get_src_zone(zone_source);
			if (src_zone && !src_zone->is_any()) {
				Row* row = zones_table.add_row();
				int col = 0;

				// add a new row for this source zone
				row->cell(col++).append(src_zone->name());

				for (const std::string& zone_destination : zone_names) {
					const DstZone* dst_zone = context.nw.get_dst_zone(zone_destination);

					if (dst_zone && !dst_zone->is_any()) {
						const RuleList::Counters counters = rules
							.filter(*src_zone, *dst_zone)
							.get_counters();

						if (counters.allowed > 0)
							row->cell(col).append(counters.allowed);
					}

					col++;
				}
			}
		}

		const Table rules_table = rules.output(firewall->make_output_options(false));

		if (args.has_option(CliCommandFlag::OutputToFile)) {
			const std::string& output_file{ args.output_file() };
			if (write_table(output_file, rules_table, ctrlc_guard))
				context.logger->info("%zu rules written to '%s'",
					rules_table.row_count(),
					output_file.c_str());
		}
		else {
			write_table(zones_table, ctrlc_guard);
			std::cout << std::endl;
			write_table(rules_table, ctrlc_guard);
		}

	}



	CliFwCheckServiceCommand::CliFwCheckServiceCommand(CliContext& context) :
		CliCommand(context, 1, 1, new CliCommandFlags({
										CliCommandFlag::OutputToFile,
										CliCommandFlag::IncludeAny,
										CliCommandFlag::ZoneFilter }))
	{
	}


	void CliFwCheckServiceCommand::do_execute(CliArgs& args, const CliCtrlcGuard& ctrlc_guard)
	{
		const Firewall* firewall = context.fw;
		if (!firewall)
			report_firewall_not_selected();

		// Decode the services
		ServiceArgs services = get_services_arg(args);
		if (services.list->empty())
			throw std::runtime_error("service not specified");

		// get all rules allowing the specified services
		const bool include_any = args.has_option(CliCommandFlag::IncludeAny) || services.list->is_any();
		const RuleList rules{
			// select all rules or rules filtered by zones
			firewall->acl().filter_if(get_zones(args))
			// keep only allow
			.filter(RuleAction::ALLOW)
			// filter rules that allow the specified services
			.filter(*services.list)
			// Remove rules that allow only "any services" unless -any is on the command line
			// (-any is assumed if the filter criteria is "any services")
			.filter_if(
				[](const Rule& rule) -> bool {
					return !rule.predicate().services().is_any();
				},
				!include_any
			)
		};

		// build the column headers, exclude any zone from the header
		const ZoneNames zone_names{ firewall->acl().zone_names() };
		Table::Headers headers{ "zones" };
		std::copy_if(
			zone_names.begin(),
			zone_names.end(),
			std::back_inserter(headers),
			[](const std::string& zone) {return !iequal(zone, "any"); }
		);
		
		Table zones_table{ headers };

		for (const std::string& zone_source : zone_names) {
			const SrcZone* src_zone = context.nw.get_src_zone(zone_source);
			if (src_zone && !src_zone->is_any()) {
				Row* row = zones_table.add_row();
				int col = 0;

				// add a new row for this source zone
				row->cell(col++).append(src_zone->name());

				for (const std::string& zone_destination : zone_names) {
					const DstZone* dst_zone = context.nw.get_dst_zone(zone_destination);

					if (dst_zone && !dst_zone->is_any()) {
						const RuleList::Counters counters = rules
							.filter(*src_zone, *dst_zone)
							.get_counters();

						if (counters.allowed > 0)
							row->cell(col).append(counters.allowed);
					}

					col++;
				}
			}
		}

		const Table rules_table = rules.output(firewall->make_output_options(false));

		if (args.has_option(CliCommandFlag::OutputToFile)) {
			const std::string& output_file{ args.output_file() };
			if (write_table(output_file, rules_table, ctrlc_guard))
				context.logger->info("rules written to '%s'",
					rules_table.row_count(),
					output_file.c_str());
		}
		else {
			write_table(zones_table, ctrlc_guard);
			std::cout << std::endl;
			write_table(rules_table, ctrlc_guard);
		}
	}


	CliFwCheckAppCommand::CliFwCheckAppCommand(CliContext& context) :
		CliCommand(context, 1, 2, new CliCommandFlags({
										CliCommandFlag::OutputToFile,
										CliCommandFlag::IncludeAny,
										CliCommandFlag::ZoneFilter }))
	{
	}


	void CliFwCheckAppCommand::do_execute(CliArgs& args, const CliCtrlcGuard& ctrlc_guard)
	{
		const Firewall* firewall = context.fw;
		if (!firewall)
			report_firewall_not_selected();

		if (!context.nw.model_options.contains(ModelOption::Application))
			throw std::runtime_error("application model is disabled");

		// Decode the applications
		const ApplicationsArgs applications{ get_applications_arg(args) };

		// Decode the services if specified
		const ServiceArgs services{ get_services_arg(args) };

		// Filter the rules by applications and optionally by services
		const bool include_any = args.has_option(CliCommandFlag::IncludeAny) || applications.list->is_any();
		const RuleList rules {
			// select all rules or rules filtered by zones
			firewall->acl().filter_if(get_zones(args))
			// keep only allow
			.filter(RuleAction::ALLOW)
			// select rules by applications and optionally by services
			.filter(*applications.list, *services.list)
			// Remove rules that allow only "any applications" unless -any is on the command line
			// (-any is assumed if the filter criteria is "any applications")
			.filter_if(
				[](const Rule& rule) -> bool {
					return !rule.predicate().applications().is_any(); },
				!include_any
			)
		};

		// Build the column headers, exclude any from the list
		const ZoneNames zone_names{ firewall->acl().zone_names().remove("any") };
		Table::Headers headers{ "zones" };
		std::copy(zone_names.begin(), zone_names.end(), std::back_inserter(headers));

		// Initialize and fill the table
		Table zones_table{ headers };

		for (const std::string& zone_source : zone_names) {
			const SrcZone* src_zone = context.nw.get_src_zone(zone_source);
			if (src_zone) {
				Row *row = zones_table.add_row();
				int col = 0;

				// add a new row for this source zone
				row->cell(col++).append(src_zone->name());

				for (const std::string& zone_destination : zone_names) {
					const DstZone* dst_zone = context.nw.get_dst_zone(zone_destination);

					if (dst_zone) {
						const size_t count = rules
							.filter(*src_zone, *dst_zone)
							.filter(RuleAction::ALLOW)
							.size();

						if (count > 0)
							row->cell(col).append(count);
					}

					col++;
				}
			}
		}

		Table rules_table = rules.output(firewall->make_output_options(false));

		if (args.has_option(CliCommandFlag::OutputToFile)) {
			const std::string& output_file{ args.output_file() };
			if (write_table(output_file, rules_table, ctrlc_guard))
				context.logger->info("rules written to '%s'",
					rules_table.row_count(),
					output_file.c_str());
		}
		else {
			write_table(zones_table, ctrlc_guard);
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
		const Firewall* firewall = context.fw;
		if (!firewall)
			report_firewall_not_selected();

		// Get the source zone (optional, can be replaced by _ or an empty string)
		const SrcZone* src_zone = get_src_zone(args, true);

		// Decode the source addresses
		const SourceAddressesArgs sources = get_source_addresses_arg(args);

		// Get the destination zone (optional, can be replaced by _ or an empty string)
		const DstZone* dst_zone = get_dst_zone(args, true);

		// Decode the destination addresses
		const DestinationAddressesArgs destinations = get_destination_addresses(args);

		// Decode the services
		const ServiceArgs services = get_services_arg(args);

		// Check if the packet is allowed
		fwm::PacketTester packet_tester{ firewall->network(), firewall->acl() };
		auto result = packet_tester.is_packet_allowed(
			src_zone,
			*sources.list,
			dst_zone,
			*destinations.list,
			*services.list,
			nullptr,
			nullptr
		);

		if (!result.second)
			context.logger->info("packets are denied");

		else {
			context.logger->info(
				string_format("rule %d %s this packet",
					result.second->id(),
					(result.first ? "allows" : "denies")
				).c_str()
			);
		}

	}

}
