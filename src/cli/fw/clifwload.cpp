/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#include "cli/fw/clifwload.h"

#include <algorithm>
#include <iostream>
#include <list>
#include <string>
#include "model/firewall.h"
#include "tools/interrupt.h"
#include "tools/io.h"
#include "tools/yesno.h"


namespace cli {

	CliFwLoadCommand::CliFwLoadCommand(CliContext& context) :
		CliCommand(context, 1, 1, new CliCommandFlags())
	{
	};


	static void extend_list(std::list<int> &destination, const std::vector<int>& source)
	{
		destination.insert(destination.end(), source.begin(), source.end());
	}


	void CliFwLoadCommand::do_execute(CliArgs& args, const CliCtrlcGuard& ctrlc_guard)
	{
		const std::string filename{ args.pop() };

		if (!rat::file_exists(filename)) {
			context.logger->error("file '%s' not found", filename.c_str());
		}
		else {
			// Load all rules
			context.logger->info("loading rules from '%s'", filename.c_str());
			LoaderStatus status = context.load_current_firewall(filename, ctrlc_guard);

			// Display load status
			context.logger->info("%zu %s out of %zu loaded",
				status.loaded_count,
				rat::pluralize(status.loaded_count, "row").c_str(),
				status.loaded_count + status.error_count + status.disabled_count
			);

			// Show rows with at least an empty zone, address, service or application
			// 1) merge all rows having an empty row
			std::list<int> problematic_rows;
			extend_list(problematic_rows, status.empty_src_zones);
			extend_list(problematic_rows, status.empty_dst_zones);
			extend_list(problematic_rows, status.empty_sources);
			extend_list(problematic_rows, status.empty_destinations);
			extend_list(problematic_rows, status.empty_services);
			extend_list(problematic_rows, status.empty_applications);
			extend_list(problematic_rows, status.empty_users);
			if (problematic_rows.size() > 0) {
				// 2) remove duplicate row numbers
				problematic_rows.sort();
				problematic_rows.unique();

				// 3) show empty rules
				std::cout
					<< " >> "
					<< problematic_rows.size()
					<< ((problematic_rows.size() > 1) ? " rows have" : " row has")
					<< " at least an undefined zone, address, service, application or user."
					<< std::endl;

				int max_rules = std::min((int)problematic_rows.size(), 10);
				std::cout
					<< " >> problems detected at "
					<< rat::pluralize(problematic_rows.size(), "row")
					<< " : ";
				for (const int rule_number : problematic_rows) {
					std::cout << std::to_string(rule_number);
					if (--max_rules != 0)
						std::cout << ", ";
					else
						break;
				}

				if (problematic_rows.size() > 10)
					std::cout << ", ...";
				std::cout << std::endl;
			}

			// Show unresolved addresses, services, applications or users
			show_unresolved(status.unresolved_addresses, "address", "es");
			show_unresolved(status.unresolved_services, "service", "s");
			show_unresolved(status.unresolved_applications, "application", "s");
			show_unresolved(status.unresolved_users, "user", "s");
		}
	};


	void CliFwLoadCommand::show_unresolved(std::list<std::string>& unresolved, const std::string& name, const std::string& plural_suffix)
	{
		if (unresolved.size() > 0) {
			// remove duplicate names
			unresolved.sort();
			unresolved.unique();

			const std::string object_name{ rat::pluralize(unresolved.size(), name, plural_suffix) };

			std::cout << " >> " << "Unresolved " << object_name << std::endl;

			bool confirm = true;
			if (unresolved.size() > 20) {
				confirm = rat::question_yesno(
					fmt::format("Display all {} {}", unresolved.size(), object_name),
					false
				);
			}

			if (confirm) {
				for (const std::string& value : unresolved)
					std::cout << "    " << value << std::endl;
			}
		}
	}

}
