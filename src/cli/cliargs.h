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


	using namespace fwm;

	using SrcZoneOptArg = std::optional<const SrcZone&>;
	using DstZoneOptArg = std::optional<const DstZone&>;
	using ZonePairOptArg = std::optional<const ZonePair>;

	class SrcAddressArgs {
	public:
		SrcAddressArgs();

		void add(const SrcAddressGroup* group);
		void add(const SrcAddress* address);
		void add(const std::string& address, IPAddressModel ip_model, bool strict_ip_parser);

		inline const SrcAddressGroup& list() const noexcept {return *_list;}

	private:
		SrcAddressGroupPtr _list;
		std::forward_list<std::unique_ptr<const SrcAddress>> _cache;
	};

	class DstAddressArgs {
	public:
		DstAddressArgs();

		void add(const DstAddressGroup* group);
		void add(const DstAddress* address);
		void add(const std::string& address, IPAddressModel ip_model, bool strict_ip_parser);

		inline const DstAddressGroup& list() const noexcept {return *_list;}

	private:
		DstAddressGroupPtr _list;
		std::forward_list<std::unique_ptr<const DstAddress>> _cache;
	};


	struct AddressArgs {
		SrcAddressArgs src_addr_args;
		DstAddressArgs dst_addr_args;
	};


	class ServiceArgs {
	public:
		ServiceArgs();

		void add(const ServiceGroup* group);
		void add(const Service* service);
		void add(const std::string& service);

		inline const ServiceGroup& list() const noexcept {return *_list;}

	private:
		ServiceGroupPtr _list;
		std::forward_list<std::unique_ptr<const Service>> _cache;
	};


	class ApplicationArgs {
	public:
		ApplicationArgs();

		void add(const ApplicationGroup* group);
		void add(const Application* application);

		inline const ApplicationGroup& list() const noexcept {return *_list;}

	private:
		ApplicationGroupPtr _list;
	};

}
