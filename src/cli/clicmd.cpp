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
#include <limits>

#include "model/tablewriter.h"
#include "tools/io.h"
#include "tools/yesno.h"
#include "fmt/core.h"

namespace cli {

	CliCommand::CliCommand(CliContext& context, int min_args, int max_args, CliCommandFlags* options) :
		context{ context },
		_min_args{ min_args },
		_max_args{ max_args },
		_flags{ options }
	{
	}


	CliCommand::CliCommand(CliContext& context) :
		CliCommand(context, 1, std::numeric_limits<int>::max(), nullptr)
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


	SrcZoneOptArg CliCommand::get_src_zone(const std::string& zone, bool optional) const
	{
		if (optional && (zone == "_" || zone.empty())) {
			return {};
		}
		else {
			const SrcZone* src_zone = context.network.get_src_zone(zone);
			if (!src_zone)
				throw std::runtime_error(fmt::format("source zone '{}' not found", zone));

			return {*src_zone};
		}
	}

	DstZoneOptArg CliCommand::get_dst_zone(const std::string& zone, bool optional) const
	{
		if (optional && (zone == "_" || zone.empty())) {
			return {};
		}
		else {
			const DstZone* dst_zone = context.network.get_dst_zone(zone);
			if (!dst_zone)
				throw std::runtime_error(fmt::format("destination zone '{}' not found", zone));

			return {*dst_zone};
		}
	}

	std::optional<const SrcZone&> CliCommand::get_src_zone(CliArgs& args, bool optional) const
	{
		return get_src_zone(args.pop(), optional);
	}


	std::optional<const DstZone&> CliCommand::get_dst_zone(CliArgs& args, bool optional) const
	{
		return get_dst_zone(args.pop(), optional);
	}


	SrcAddressArgs CliCommand::get_source_addresses(const std::vector<std::string>& addresses) const
	{
		SrcAddressArgs src_address_args;

		for (const std::string& addr : addresses) {
			const SrcAddress* address{ context.network.get_src_address(addr) };

			if (address) {
				src_address_args.add(address);
			}
			else {
				const SrcAddressGroup* group{ context.network.get_src_address_group(addr) };
				if (group) {
					src_address_args.add(group);
				}
				else {
					// Try to convert to an IP address.
					// Allocate a temporary address that is deleted automatically.
					src_address_args.add(
							addr,
							context.network.config().ip_model,
							context.network.config().strict_ip_parser
					);
				}
			}
		}

		return src_address_args;
	}

	DstAddressArgs CliCommand::get_destination_addresses(const std::vector<std::string>& addresses) const
	{
		DstAddressArgs dst_address_args;

		for (const std::string& addr : addresses) {
			const DstAddress* address{ context.network.get_dst_address(addr) };

			if (address) {
				dst_address_args.add(address);
			}
			else {
				const DstAddressGroup* group{ context.network.get_dst_address_group(addr) };
				if (group) {
					dst_address_args.add(group);
				}
				else {
					// Try to convert to an IP address.
					// Allocate a temporary address that is deleted automatically.
					dst_address_args.add(
							addr,
							context.network.config().ip_model,
							context.network.config().strict_ip_parser
					);
				}
			}
		}

		return dst_address_args;
	}


	SrcAddressArgs CliCommand::get_source_addresses_arg(CliArgs& args) const
	{
		SrcAddressArgs src_address_args;

		std::vector<std::string> addr_args;
		if (args.size() > 0 && rat::split(args.pop(), ';', addr_args) > 0) {
			src_address_args = get_source_addresses(addr_args);
		}

		return src_address_args;
	}


	DstAddressArgs CliCommand::get_destination_addresses(CliArgs& args) const
	{
		DstAddressArgs dst_address_args;

		std::vector<std::string> addr_args;
		if (args.size() > 0 && rat::split(args.pop(), ';', addr_args) > 0) {
			dst_address_args = get_destination_addresses(addr_args);
		}

		return dst_address_args;
	}


	AddressArgs CliCommand::get_addresses_arg(CliArgs & args) const
	{
		AddressArgs address_args;

		std::vector<std::string> addr_args;
		if (args.size() > 0 && rat::split(args.pop(), ';', addr_args) > 0) {
			address_args.src_addr_args = get_source_addresses(addr_args);
			address_args.dst_addr_args = get_destination_addresses(addr_args);
		}

		return address_args;
	}


	ServiceArgs CliCommand::get_services_arg(CliArgs& args) const
	{
		ServiceArgs service_args;

		std::vector<std::string> svc_args;
		if (args.size() > 0 && rat::split(args.pop(), ';', svc_args) > 0) {
			for (const std::string& svc_arg : svc_args) {
				const Service* service{ context.network.get_service(svc_arg) };

				if (service) {
					service_args.add(service);
				}
				else {
					const ServiceGroup* group{ context.network.get_service_group(svc_arg) };
					if (group) {
						service_args.add(group);
					}
					else {
						// The service is not available, we allocate a temporary service
						// that will deleted automatically.
						service_args.add(svc_arg);
					}
				}
			}
		}

		return service_args;
	}


	ApplicationArgs CliCommand::get_applications_arg(CliArgs& args) const
	{
		ApplicationArgs application_args;

		std::vector<std::string> app_args;
		if (args.size() > 0 && rat::split(args.pop(), ';', app_args) > 0) {
			for (const std::string& app_arg : app_args) {
				const Application* application{ context.network.get_application(app_arg) };

				if (application) {
					application_args.add(application);
				}
				else {
					const ApplicationGroup* group{ context.network.get_application_group(app_arg) };
					if (group) {
						application_args.add(group);
					}
					else {
						throw std::runtime_error("application not defined");
					}
				}
			}
		}

		return application_args;
	}


	ZonePairOptArg CliCommand::get_zones_filter(const CliArgs& args) const
	{
		if (args.has_option(CliCommandFlag::ZoneFilter)) {
			// get the source zone available on the command line
			auto src_zone = get_src_zone(args.src_zone(), false);
			if (!src_zone.has_value())
				throw std::runtime_error("invalid source zone");

			// get the destination zone available on the command line
			auto dst_zone = get_dst_zone(args.dst_zone(), false);
			if (!dst_zone.has_value())
				throw std::runtime_error("invalid destination zone");

			return ZonePair{
				src_zone.value(),
				dst_zone.value()
			};
		}
		else {
			return {};
		}
	}


	void CliCommand::report_invalid_command_line()
	{
		throw std::runtime_error("invalid command line");
	}


	void CliCommand::report_unknown_cli_option(const std::string& option)
	{
		throw std::runtime_error(fmt::format("invalid command line option '{}'", option));
	}


	void CliCommand::report_unknown_model_option(const std::string& option)
	{
		throw std::runtime_error(fmt::format("invalid model option '{}'", option));
	}


	void CliCommand::report_invalid_rule_id()
	{
		throw std::runtime_error("invalid rule id");
	}


	void CliCommand::report_rule_id_not_found(int rule_id)
	{
		throw std::runtime_error(fmt::format("rule id '{}' not found", rule_id));
	}


	bool CliCommand::ask_write_to_file(const std::string & filename)
	{
		bool overwrite = true;

		if (rat::file_exists(filename)) {
			overwrite = rat::question_yesno(
				fmt::format("Do you want to overwrite file '{}'", filename),
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
		const bool overwrite = ask_write_to_file(filename);

		if (overwrite) {
			std::ofstream ofs{ filename, std::ofstream::out };
			if (!ofs.good()) {
				throw std::runtime_error("can't open file");
			}

			if (rat::iends_with(filename, ".csv")) {
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
