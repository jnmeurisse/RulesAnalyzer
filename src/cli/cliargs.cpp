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
		_list{ std::make_unique<SrcAddressGroup>("$root-src-addr-args") },
		_cache()
	{
	}


	void SrcAddressArgs::add(const SrcAddressGroup* group)
	{
		_list->add_member(group);
	}


	void SrcAddressArgs::add(const SrcAddress* address)
	{
		_list->add_member(address);
	}


	void SrcAddressArgs::add(const std::string& address, fwm::IPAddressModel ip_model, bool strict_ip_parser)
	{
		_cache.push_front(
			std::unique_ptr<const SrcAddress>(SrcAddress::create("", address, ip_model, strict_ip_parser))
		);

		add(_cache.front().get());
	}


	DstAddressArgs::DstAddressArgs() :
		_list{ std::make_unique<DstAddressGroup>("$root-dst-addr-args") },
		_cache()
	{
	}


	void DstAddressArgs::add(const DstAddressGroup* group)
	{
		_list->add_member(group);
	}


	void DstAddressArgs::add(const DstAddress* address)
	{
		_list->add_member(address);
	}


	void DstAddressArgs::add(const std::string& address, fwm::IPAddressModel ip_model, bool strict_ip_parser)
	{
		_cache.push_front(
			std::unique_ptr<const DstAddress>(DstAddress::create("", address, ip_model, strict_ip_parser))
		);

		add(_cache.front().get());
	}


	ServiceArgs::ServiceArgs() :
		_list{ std::make_unique<ServiceGroup>("$root-svc-args") },
		_cache()
	{
	}


	void ServiceArgs::add(const ServiceGroup* group)
	{
		_list->add_member(group);
	}


	void ServiceArgs::add(const Service* service)
	{
		_list->add_member(service);
	}


	void ServiceArgs::add(const std::string& service)
	{
		_cache.push_front(
			std::unique_ptr<const Service>(Service::create("", service))
		);

		add(_cache.front().get());
	}


	ApplicationArgs::ApplicationArgs() :
		_list{ std::make_unique<ApplicationGroup>("$root-app-args") }
	{
	}


	void ApplicationArgs::add(const ApplicationGroup* group)
	{
		_list->add_member(group);
	}


	void ApplicationArgs::add(const Application* application)
	{
		_list->add_member(application);
	}

}
