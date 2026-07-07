/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#include "cli/cliargs.h"

#include <algorithm>
#include <stdexcept>
#include <map>
#include "fmt/core.h"

namespace cli {

	static const std::map<CliCommandFlag, std::string> OPTION_TEXTS {
		{ CliCommandFlag::OutputToFile, "-o" },
		{ CliCommandFlag::ZoneFilter,   "-z" },
		{ CliCommandFlag::IncludeAny,   "-any" }
	};


	const std::string& CliCommandFlags::to_string(CliCommandFlag option)
	{
		return OPTION_TEXTS.at(option);
	}


	CliArgs::CliArgs(const std::vector<std::string>& args) :
		std::deque<std::string>()
	{
		bool output_option = false;		// set to true when a -o is found
		int zone_option = 0;			// counter used when a -z is found

		for (const std::string& arg : args) {
			if (output_option) {
				_output_filename = arg;
				output_option = false;
			}
			else if (zone_option == 2) {
				_src_zone = arg;
				zone_option -= 1;
			}
			else if (zone_option == 1) {
				_dst_zone = arg;
				zone_option -= 1;
			}
			else if (arg.compare("-o") == 0) {
				if (_flags.contains(CliCommandFlag::OutputToFile))
					throw std::runtime_error("duplicate -o option");

				_flags.add(CliCommandFlag::OutputToFile);
				output_option = true;
			}
			else if (arg.compare("-any") == 0) {
				if (_flags.contains(CliCommandFlag::IncludeAny))
					throw std::runtime_error("duplicate -any option");

				_flags.add(CliCommandFlag::IncludeAny);
			}
			else if (arg.compare("-z") == 0) {
				if (zone_option)
					throw std::runtime_error("duplicate -z option");

				_flags.add(CliCommandFlag::ZoneFilter);
				zone_option = 2;
			}
			else if (arg[0] == '-') {
				throw std::runtime_error(fmt::format("invalid command line option {}", arg));
			}
			else {
				this->push_back(arg);
			}
		}

		if (output_option)
			throw std::runtime_error("missing filename in option -o");

		if (zone_option > 0)
			throw std::runtime_error("missing zone in option -z");
	}


	size_t CliArgs::size() const
	{
		return std::deque<std::string>::size();
	}


	bool CliArgs::empty() const
	{
		return std::deque<std::string>::size() == 0;
	}


	const std::string& CliArgs::front() const
	{
		return std::deque<std::string>::front();
	}


	std::string CliArgs::pop()
	{
		std::string arg = front();
		std::deque<std::string>::pop_front();

		return arg;
	}


	bool CliArgs::has_option(CliCommandFlag option) const
	{
		return _flags.contains(option);
	}


	SrcAddressArgs::SrcAddressArgs() :
		fwm::SrcAddressGroup("$root-src-addr-args")
	{
	}


	void SrcAddressArgs::add(fwm::SrcAddressPtr address)
	{
		add_member(address);
	}


	void SrcAddressArgs::add(fwm::SrcAddressGroupPtr group)
	{
		add_member(group);
	}


	void SrcAddressArgs::add(const std::string& address, fwm::IPAddressModel ip_model, bool strict_ip_parser)
	{
		add_member(fwm::SrcAddress::create("", address, ip_model, strict_ip_parser));
	}


	DstAddressArgs::DstAddressArgs() :
		fwm::DstAddressGroup( "$root-dst-addr-args")
	{
	}


	void DstAddressArgs::add(fwm::DstAddressPtr address)
	{
		add_member(address);
	}


	void DstAddressArgs::add(fwm::DstAddressGroupPtr group)
	{
		add_member(group);
	}


	void DstAddressArgs::add(const std::string& address, fwm::IPAddressModel ip_model, bool strict_ip_parser)
	{
		add_member(fwm::DstAddress::create("", address, ip_model, strict_ip_parser));
	}


	ServiceArgs::ServiceArgs() :
		fwm::ServiceGroup("$root-svc-args")
	{
	}


	void ServiceArgs::add(fwm::ServicePtr service)
	{
		add_member(service);
	}


	void ServiceArgs::add(fwm::ServiceGroupPtr group)
	{
		add_member(group);
	}


	void ServiceArgs::add(const std::string& service)
	{
		add_member(fwm::Service::create("", service));
	}


	ApplicationArgs::ApplicationArgs() :
		fwm::ApplicationGroup("$root-app-args")
	{
	}


	void ApplicationArgs::add(fwm::ApplicationPtr application)
	{
		add_member(application);
	}


	void ApplicationArgs::add(fwm::ApplicationGroupPtr group)
	{
		add_member(group);
	}

}
