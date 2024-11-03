/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#include "cli/clicontext.h"

#include <stdexcept>
#include "model/firewall.h"
#include "model/network.h"
#include "ostore/objectstore.h"
#include "ostore/firewallfactory.h"
#include "fmt/core.h"


namespace cli {

	cli::CliContext::CliContext(const OstoreConfig& config) :
		ostore{ fos::ObjectStore{ config } },
		network{ fwm::Network{ config.model_config } },
		ctrlc_handler{},
		logger{ Logger::get_logger() },
		_current_firewall{ nullptr },
		_factory{ fos::FirewallFactory{ ostore, config.loader_config } }
	{
	}



	Firewall* CliContext::get_firewall(const std::string& name) const
	{
		return network.get(name);
	}


	Firewall& CliContext::add_firewall(const std::string& name)
	{
		if (get_firewall(name))
			throw std::runtime_error(fmt::format("firewall '{}' already exists", name));

		// create a new firewall
		Firewall* firewall = new Firewall(name, network);

		// add this firewall to the network
		network.add(firewall);

		// and make it current.
		_current_firewall = firewall;

		return *_current_firewall;
	}


	void CliContext::delete_firewall(const std::string& name)
	{
		if (!get_firewall(name))
			throw std::runtime_error("firewall not found");

		if (_current_firewall && _current_firewall->name() == name)
			throw std::runtime_error("firewall is currently selected, it can not be deleted");

		// delete it from the network
		network.del(name);
	}


	Firewall& CliContext::get_current_firewall() const
	{
		if (!_current_firewall)
			throw std::runtime_error("a firewall is not selected");

		return *_current_firewall;
	}


	void CliContext::set_current_firewall(Firewall* firewall)
	{
		_current_firewall = firewall;
	}


	void CliContext::clone_current_firewall(const std::string& name)
	{
		if (get_firewall(name))
			throw std::runtime_error(fmt::format("firewall '{}' already exists", name));

		network.add(_factory.clone(get_current_firewall(), name));
	}


	LoaderStatus CliContext::load_current_firewall(const std::string& filename, const CliCtrlcGuard& ctrlc_guard)
	{
		Firewall& firewall = get_current_firewall();

		// Clear all existing rules
		firewall.clear();

		// Load all rules and return a status
		return _factory.load(firewall, filename, ctrlc_guard.get_interrupt_cb());
	}


	const SrcZone* CliContext::get_any_src_zone() const
	{
		return network.get_src_zone("any");
	}


	const DstZone* CliContext::get_any_dst_zone() const
	{
		return network.get_dst_zone("any");
	}

}


