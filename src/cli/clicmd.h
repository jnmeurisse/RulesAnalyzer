/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#pragma once
#include "global.h"

#include <list>
#include <memory>
#include <vector>

#include "cli/cliargs.h"
#include "cli/clicontext.h"
#include "model/address.h"
#include "model/application.h"
#include "model/service.h"
#include "model/zone.h"


namespace cli {

	/**
	* CliCommand is the base abstract class for all commands
	*/
	class CliCommand abstract
	{
	public:
		/* Constructs a cli command having between 'min_args' and
		   'max_args' arguments and an optional set of arguments.
		*/
		CliCommand(CliContext& context, int min_args, int max_args, CliCommandFlags* options);

		/* Constructs a cli command that has no restriction on the
		   number of arguments.
		*/
		CliCommand(CliContext& context);

		/* Destructor
		*/
		virtual ~CliCommand() {};

		/* Executes the command.  The method checks whether the number of
		   arguments is valid (between min_args and max_args) before calling
		   do_execute.  The method outputs an error message on the console
		   if the number of arguments is not valid.
		*/
		void execute(CliArgs& args, const CliCtrlcGuard& ctrlc_guard);

	protected:
		// a reference to the context
		CliContext& context;

		/* executes the command.
		*/
		virtual void do_execute(CliArgs& args, const CliCtrlcGuard& ctrlc_guard) = 0;

		/* Convenient methods that get source or destination zones from the argument list.
		*/
		SrcZoneOptArg get_src_zone(CliArgs& args, bool optional) const;
		DstZoneOptArg get_dst_zone(CliArgs& args, bool optional) const;

		// Get a list of source addresses from the command line.
		SrcAddressArgs get_source_addresses_arg(CliArgs& args) const;

		// Get a list of destination addresses from the command line.
		DstAddressArgs get_destination_addresses(CliArgs& args) const;

		// Get a list of source and destination addresses from the next command line argument.
		AddressArgs get_addresses_arg(CliArgs& args) const;

		// Get a list of services from the next command line argument.
		ServiceArgs get_services_arg(CliArgs& args) const;

		// Get a list of applications from the next command line argument.
		ApplicationArgs get_applications_arg(CliArgs& args) const;

		// Return the source and destination zones given by the option -z
		ZonePairOptArg get_zones_filter(const CliArgs& args) const;

		// common error messages
		static void report_invalid_command_line();
		static void report_unknown_cli_option(const std::string& option);
		static void report_unknown_model_option(const std::string& option);
		static void report_invalid_rule_id();
		static void report_rule_id_not_found(int rule_id);

		/* Asks if a file should be overwritten.
		*/
		static bool ask_write_to_file(const std::string& filename);

		/* Writes a table to std::cout.
		*/
		void write_table(const Table& table, const CliCtrlcGuard& ctrlc_guard);

		/* Writes a table to a file.
		*/
		bool write_table(const std::string& filename, const Table& table, const CliCtrlcGuard& ctrlc_guard);

	private:
		// min and max expected number of arguments
		const int _min_args;
		const int _max_args;

		// set of supported command line options
		const std::unique_ptr<CliCommandFlags> _flags;

		// Convenient methods that get source or destination zone by name.
		SrcZoneOptArg get_src_zone(const std::string& zone, bool optional) const;
		DstZoneOptArg get_dst_zone(const std::string& zone, bool optional) const;

		// Convenient methods that get source and destination addresses.
		SrcAddressArgs get_source_addresses(const std::vector<std::string>& addresses) const;
		DstAddressArgs get_destination_addresses(const std::vector<std::string>& addresses) const;
	};

}
