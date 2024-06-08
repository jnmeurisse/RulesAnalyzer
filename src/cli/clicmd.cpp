/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#include "clicmd.h"

#include <fstream>
#include <iostream>
#include <stdexcept>

#include "model/tablewriter.h"
#include "tools/strutil.h"
#include "tools/io.h"
#include "tools/yesno.h"


namespace cli {

	CliCommand::CliCommand(CliContext& context, int min_args, int max_args, CliCommandFlags* options) :
		context{ context },
		_min_args{ min_args },
		_max_args{ max_args },
		_flags{ options }
	{
	}


	CliCommand::CliCommand(CliContext& context) :
		CliCommand(context, 1, INT_MAX, nullptr)
	{
	}


	void CliCommand::execute(CliArgs& args, const CliCtrlcGuard& ctrlc_guard)
	{
		if (args.size() < _min_args)
			std::cout << "missing command argument." << std::endl;

		else if (args.size() > _max_args)
			report_invalid_command_line();

		else {
			for (const CliCommandFlag flag : args.flags()) {
				if (_flags && !_flags->contains(flag)) {
					report_unknown_cli_option(CliCommandFlags::to_string(flag));
				}
			}

			do_execute(args, ctrlc_guard);
		}
	}


	const SrcZone* CliCommand::get_src_zone(const std::string& zone, bool optional) const
	{
		if (optional && (zone == "_" || zone.empty())) {
			return nullptr;
		}
		else {
			const SrcZone* src_zone = context.nw.get_src_zone(zone);
			if (!src_zone)
				throw std::runtime_error(string_format("source zone '%s' not found", zone.c_str()));

			return src_zone;
		}
	}

	const DstZone* CliCommand::get_dst_zone(const std::string& zone, bool optional) const
	{
		if (optional && (zone == "_" || zone.empty())) {
			return nullptr;
		}
		else {
			const DstZone* dst_zone = context.nw.get_dst_zone(zone);
			if (!dst_zone)
				throw std::runtime_error(string_format("destination zone '%s' not found", zone.c_str()));

			return dst_zone;
		}
	}

	const SrcZone* CliCommand::get_src_zone(CliArgs& args, bool optional) const
	{
		return get_src_zone(args.pop(), optional);
	}


	const DstZone* CliCommand::get_dst_zone(CliArgs& args, bool optional) const
	{
		return get_dst_zone(args.pop(), optional);
	}


	SourceAddressesArgs CliCommand::get_source_addresses(std::vector<std::string>& addresses) const
	{
		SourceAddressesArgs src_addresses;

		for (const std::string& addr : addresses) {
			const SrcAddress* address{ context.nw.get_src_address(addr) };

			if (address) {
				src_addresses.list->add_member(address);
			}
			else {
				const SrcAddressGroup* group{ context.nw.get_src_address_group(addr) };
				if (group) {
					src_addresses.list->add_member(group);
				}
				else {
					// Try to convert to an IP address.
					// Allocate a temporary address that is deleted automatically.
					src_addresses.cache.push_back(std::unique_ptr<SrcAddress>(SrcAddress::create("", addr, true)));
					src_addresses.list->add_member(src_addresses.cache.back().get());
				}
			}
		}

		return src_addresses;
	}

	DestinationAddressesArgs CliCommand::get_destination_addresses(std::vector<std::string>& addresses) const
	{
		DestinationAddressesArgs dst_addresses;

		for (const std::string& addr : addresses) {
			const DstAddress* address{ context.nw.get_dst_address(addr) };

			if (address) {
				dst_addresses.list->add_member(address);
			}
			else {
				const DstAddressGroup* group{ context.nw.get_dst_address_group(addr) };
				if (group) {
					dst_addresses.list->add_member(group);
				}
				else {
					// Try to convert to an IP address.
					// Allocate a temporary address that is deleted automatically.
					dst_addresses.cache.push_back(std::unique_ptr<DstAddress>(DstAddress::create("", addr, true)));
					dst_addresses.list->add_member(dst_addresses.cache.back().get());
				}
			}
		}

		return dst_addresses;
	}


	SourceAddressesArgs CliCommand::get_source_addresses_arg(CliArgs& args) const
	{
		SourceAddressesArgs src_addresses;

		std::vector<std::string> addr_args;
		if (args.size() > 0 && split(args.pop(), '&', addr_args) > 0) {
			src_addresses = get_source_addresses(addr_args);
		}

		return src_addresses;
	}


	DestinationAddressesArgs CliCommand::get_destination_addresses(CliArgs& args) const
	{
		DestinationAddressesArgs dst_addresses;

		std::vector<std::string> addr_args;
		if (args.size() > 0 && split(args.pop(), '&', addr_args) > 0) {
			dst_addresses = get_destination_addresses(addr_args);
		}

		return dst_addresses;
	}


	AddressesArgs CliCommand::get_addresses_arg(CliArgs & args) const
	{
		AddressesArgs addresses;

		std::vector<std::string> addr_args;
		if (args.size() > 0 && split(args.pop(), '&', addr_args) > 0) {
			addresses.src = get_source_addresses(addr_args);
			addresses.dst = get_destination_addresses(addr_args);
		}

		return addresses;
	}


	ServiceArgs CliCommand::get_services_arg(CliArgs& args) const
	{
		ServiceArgs services;

		std::vector<std::string> svc_args;
		if (args.size() > 0 && split(args.pop(), '&', svc_args) > 0) {
			for (const std::string& svc_arg : svc_args) {
				const Service* service{ context.nw.get_service(svc_arg) };

				if (service) {
					services.list->add_member(service);
				}
				else {
					const ServiceGroup* group{ context.nw.get_service_group(svc_arg) };
					if (group) {
						services.list->add_member(group);
					}
					else {
						// The service is not available, we allocate a temporary service
						// that will deleted automatically.
						services.cache.push_back(std::unique_ptr<Service>(Service::create("", svc_arg)));
						services.list->add_member(services.cache.back().get());
					}
				}
			}
		}

		return services;
	}


	ApplicationsArgs CliCommand::get_applications_arg(CliArgs& args) const
	{
		ApplicationsArgs applications;

		std::vector<std::string> app_args;
		if (args.size() > 0 && split(args.pop(), '&', app_args) > 0) {
			for (const std::string& app_arg : app_args) {
				const Application* application{ context.nw.get_application(app_arg) };

				if (application) {
					applications.list->add_member(application);
				}
				else {
					const ApplicationGroup* group{ context.nw.get_application_group(app_arg) };
					if (group) {
						applications.list->add_member(group);
					}
					else {
						throw std::runtime_error("application not defined");
					}
				}
			}
		}

		return applications;
	}


	ZonePair CliCommand::get_zones(const CliArgs& args) const
	{
		ZonePair zones{ nullptr, nullptr };

		if (args.has_option(CliCommandFlag::ZoneFilter)) {
			// get the source zone option
			zones.src_zone = get_src_zone(args.src_zone(), false);
			if (!zones.src_zone) throw std::runtime_error("invalid source zone");

			// get the destination zone option
			zones.dst_zone = get_dst_zone(args.dst_zone(), false);
			if (!zones.dst_zone) throw std::runtime_error("invalid destination zone");
		}

		return zones;
	}


	void CliCommand::report_invalid_command_line()
	{
		throw std::runtime_error("invalid command line");
	}


	void CliCommand::report_unknown_cli_option(const std::string& option)
	{
		throw std::runtime_error(string_format("invalid command line option '%s'", option.c_str()));
	}


	void CliCommand::report_firewall_not_selected()
	{
		throw std::runtime_error("a firewall is not selected");
	}


	void CliCommand::report_unknown_model_option(const std::string& option)
	{
		throw std::runtime_error(string_format("invalid model option '%s'", option.c_str()));
	}


	void CliCommand::report_invalid_rule_id()
	{
		throw std::runtime_error("invalid rule id");
	}


	void CliCommand::report_rule_id_not_found(int rule_id)
	{
		throw std::runtime_error(string_format("rule id '%d' not found", rule_id));
	}


	bool CliCommand::ask_write_to_file(const std::string & filename)
	{
		bool overwrite = true;

		if (file_exists(filename)) {
			overwrite = question_yesno(
				string_format("Do you want to overwrite file '%s'", filename.c_str()),
				false
			);
		}

		return overwrite;
	}


	void CliCommand::write_table(const Table& table, const CliCtrlcGuard& ctrlc_guard)
	{
		TableTxtWriter writer{ table};
		writer
			.margin(4)
			.write(std::cout, ctrlc_guard.get_interrupt_cb());
	}


	bool CliCommand::write_table(const std::string& filename, const Table& table, const CliCtrlcGuard& ctrlc_guard)
	{
		bool overwrite = true;

		if (file_exists(filename)) {
			overwrite = question_yesno(
				string_format("Do you want to overwrite file '%s'", filename.c_str()),
				false
			);
		}

		if (overwrite) {
			std::ofstream ofs{ filename, std::ofstream::out };
			if (!ofs.good()) {
				throw std::runtime_error("can't open file");
			}

			if (iends_with(filename, ".csv")) {
				TableCsvWriter writer(table);
				writer
					.newline('\n')
					.separator(',')
					.write(ofs, ctrlc_guard.get_interrupt_cb());
			}
			else {
				TableTxtWriter writer{ table };
				writer.write(ofs, ctrlc_guard.get_interrupt_cb());
			}
		}

		return overwrite;
	}

}
