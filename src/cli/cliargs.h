/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#pragma once
#include "global.h"

#include <forward_list>
#include <deque>
#include <set>
#include <string>
#include <vector>

#include "model/address.h"
#include "model/application.h"
#include "model/service.h"
#include "model/zone.h"
#include "tools/options.h"
#include "tools/optional.h"


namespace cli {

	enum class CliCommandFlag {
		OutputToFile,           // -o   : output to file option
		IncludeAny,             // -any : include "any" objects option
		ZoneFilter              // -z   : zone filter option
	};


	class CliCommandFlags : public rat::Options<CliCommandFlag>
	{
	public:
		using Options<CliCommandFlag>::Options;

		/* returns an option as a string.
		*/
		static const std::string& to_string(CliCommandFlag option);
	};


	/**
	* CliArgs provides the command line arguments as a queue of strings.
	*/
	class CliArgs : private std::deque<std::string>
	{
	public:
		/* Initializes from a vector of arguments.
		*/
		CliArgs(const std::vector<std::string>& args);

		/* Returns the number of arguments remaining in the queue.
		*/
		size_t size() const;

		/* Returns true if the command line arguments is empty.
		*/
		bool empty() const;

		/* Returns the first argument without removing it.
		*/
		const std::string& front() const;

		/* Removes and returns the first argument.
		*/
		std::string pop();

		/* Returns the -o filename option.
		*/
		const std::string& output_file() const { return _output_filename; }

		/* Returns the source zone in -zone option.
		*/
		const std::string& src_zone() const { return _src_zone; }

		/* Returns the destination zone in -zone option.
		*/
		const std::string& dst_zone() const { return _dst_zone; }

		/* Returns other flags present on the command line.
		*/
		const std::vector<CliCommandFlag> flags() const { return _flags.options(); }

		/* Returns true whether an option is present on the command line.
		*/
		bool has_option(CliCommandFlag option) const;

	private:
		// -o output filename
		std::string _output_filename;

		// -zone zones
		std::string _src_zone;
		std::string _dst_zone;

		// other flags
		CliCommandFlags _flags;
	};


	using ZonePairOptArg = std::optional<const fwm::ZonePair>;

	class SrcAddressArgs : public fwm::SrcAddressGroup {
	public:
		SrcAddressArgs();

		void add(fwm::SrcAddressPtr address);
		void add(fwm::SrcAddressGroupPtr group);
		void add(const std::string& address, fwm::IPAddressModel ip_model, bool strict_ip_parser);
	};

	using SrcAddressArgsPtr = std::unique_ptr<SrcAddressArgs>;


	class DstAddressArgs : public fwm::DstAddressGroup {
	public:
		DstAddressArgs();

		void add(fwm::DstAddressPtr address);
		void add(fwm::DstAddressGroupPtr group);
		void add(const std::string& address, fwm::IPAddressModel ip_model, bool strict_ip_parser);
	};

	using DstAddressArgsPtr = std::unique_ptr<DstAddressArgs>;


	struct AddressArgs {
		SrcAddressArgsPtr src_addr_args;
		DstAddressArgsPtr dst_addr_args;
	};


	class ServiceArgs : public fwm::ServiceGroup {
	public:
		ServiceArgs();

		void add(fwm::ServicePtr service);
		void add(fwm::ServiceGroupPtr group);
		void add(const std::string& service);
	};

	using ServiceArgsPtr = std::unique_ptr<ServiceArgs>;


	class ApplicationArgs : public fwm::ApplicationGroup {
	public:
		ApplicationArgs();

		void add(fwm::ApplicationPtr application);
		void add(fwm::ApplicationGroupPtr group);
	};

	using ApplicationArgsPtr = std::unique_ptr<ApplicationArgs>;

}
