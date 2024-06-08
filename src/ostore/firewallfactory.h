/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#pragma once

#include <string>
#include <tuple>

#include "config.h"
#include "objectstore.h"
#include "policyreader.h"
#include "ruleobject.h"

#include "model/application.h"
#include "model/address.h"
#include "model/firewall.h"
#include "model/network.h"
#include "model/service.h"
#include "model/user.h"
#include "model/zone.h"

#include "tools/interrupt.h"
#include "tools/logger.h"

namespace fos {

	struct LoaderStatus {
		size_t loaded_count;
		size_t error_count;
		size_t disabled_count;

		std::vector<int> empty_sources;
		std::vector<int> empty_destinations;
		std::vector<int> empty_services;
		std::vector<int> empty_applications;
		std::vector<int> empty_src_zones;
		std::vector<int> empty_dst_zones;
		std::vector<int> empty_users;

		std::list<std::string> unresolved_addresses;
		std::list<std::string> unresolved_services;
		std::list<std::string> unresolved_applications;
		std::list<std::string> unresolved_users;

		LoaderStatus() :
			loaded_count{ 0 },
			error_count{ 0 },
			disabled_count{ 0 },
			empty_sources{},
			empty_destinations{},
			empty_services{},
			empty_applications{},
			empty_src_zones{},
			empty_dst_zones{},
			empty_users{},
			unresolved_addresses{},
			unresolved_services{},
			unresolved_applications{},
			unresolved_users{}
		{}
	};



	class FirewallFactory
	{
	public:
		FirewallFactory(const fos::ObjectStore& object_store, const loader_config& loader_config);
		LoaderStatus load(Firewall& fw, const std::string& csv_filename, f_interrupt_cb interrupt_cb);
		Firewall* clone(const Firewall& fw, const std::string& name) const;

	private:
		// A reference to the storage area that contains all objects : addresses, zones, ...
		const ObjectStore& _object_store;

		// A reference to the loader configuration.
		const loader_config& _loader_config;

		// A reference to the application logger
		Logger* const _logger;

		/* Loads all rules
		*/
		LoaderStatus load_rules(Firewall& fw, PolicyReader& reader);

		/* Builds the source address group used by the given rule.
		*/
		SrcAddressGroupPtr build_src_address_group(Network& nw, const fos::RuleObject& rule, LoaderStatus& status);

		/* Builds the destination address list used by the given rule.
		*/
		DstAddressGroupPtr build_dst_address_group(Network& nw, const fos::RuleObject& rule, LoaderStatus& status);

		/* Builds the service list used by the given rule.
		*/
		ServiceGroupPtr build_service_group(Network& nw, const fos::RuleObject& rule, LoaderStatus& status);

		/* Builds the application list used by the given rule.
		*/
		ApplicationGroupPtr build_application_group(Network& nw, const fos::RuleObject& rule, LoaderStatus& status) const;

		/* Builds the source zone list used by the given rule.
		*/
		SrcZoneGroupPtr build_src_zone_group(Network& nw, const fos::RuleObject& rule, LoaderStatus& status) const;

		/* Builds the destination zone list used by the given rule.
		*/
		DstZoneGroupPtr build_dst_zone_group(Network& nw, const fos::RuleObject& rule, LoaderStatus& status) const;

		/* Builds the user list used by the given rule.
		*/
		UserGroupPtr build_user_group(Network& nw, const fos::RuleObject& rule, LoaderStatus& status) const;

		/* Adds a rule to a firewall.
		*/
		bool add_rule(Firewall& fw, const fos::RuleObject& rule, LoaderStatus& status);

		/* Adds a deny all policy to a firewall
		*/
		bool add_deny_all_rule(Firewall& fw) const;
	};

}
