/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#include <memory>

#include "mnode.h"
#include "packettester.h"


namespace fwm {

	PacketTester::PacketTester(const Network& nw, const Acl& acl) :
		_nw{ nw },
		_acl{ acl }
	{
	}

	std::pair<bool, const Rule*> PacketTester::is_packet_allowed(
		const SrcZone* src_zone,
		const SrcAddressGroup& src_addr,
		const DstZone* dst_zone,
		const DstAddressGroup& dst_addr,
		const ServiceGroup& svc,
		const ApplicationGroup* app,
		const UserGroup* usr
	) const
	{
		Predicate::BddOptions bdd_options;

		if (src_zone)
			bdd_options.add(Predicate::BddOption::SourceZone);
		if (dst_zone)
			bdd_options.add(Predicate::BddOption::DestinationZone);
		if (app)
			bdd_options.add(Predicate::BddOption::Application);
		if (usr)
			bdd_options.add(Predicate::BddOption::User);

		// Prepare sources
		SrcZoneGroupPtr source_zones{ new SrcZoneGroup("", src_zone? src_zone : _nw.get_src_zone("any")) };
		SrcAddressGroupPtr source_addresses{ new SrcAddressGroup(src_addr) };

		Sources sources{ source_zones, source_addresses, false};

		// Prepare destinations
		DstZoneGroupPtr destinations_zones{ new DstZoneGroup("", dst_zone ? dst_zone : _nw.get_dst_zone("any")) };
		DstAddressGroupPtr destination_addresses{ new DstAddressGroup(dst_addr) };

		Destinations destinations{ destinations_zones, destination_addresses, false };

		// Prepare the services
		ServiceGroupPtr services{ new ServiceGroup(svc) };

		// Prepare the applications
		ApplicationGroupPtr applications{ app
			? new ApplicationGroup(*app)
			: new ApplicationGroup("", _nw.get_application("any"))
		};

		// Prepare the users
		UserGroupPtr users{ usr
			? new UserGroup(*usr)
			: new UserGroup("", _nw.get_user("any"))
		};

		// Create the test predicate
		Predicate test_predicate{
			sources,
			destinations,
			services,
			applications,
			users
		};

		Bddnode test_bdd{ test_predicate.make_bdd(bdd_options) };

		for (const auto rule : _acl) {
			// Is the traffic defined by the test predicate accepted by this rule ?
			if (test_bdd.is_subset(Bddnode(rule->predicate().make_bdd(bdd_options)))) {
				return std::make_pair(rule->action() == RuleAction::ALLOW, rule);
			}
		}

		// Traffic is denied
		return std::make_pair(false, nullptr);
	}

}
