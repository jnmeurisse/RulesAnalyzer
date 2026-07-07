/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#include "model/packettester.h"

#include <memory>
#include <utility>
#include "model/mnode.h"
#include "model/predicate.h"


namespace fwm {

	PacketTester::PacketTester(const RuleList& acl) :
		_acl{ acl }
	{
	}

	std::pair<bool, const Rule*> PacketTester::is_packet_allowed(
		const SrcZonePtr src_zone,
		const SrcAddressGroupPtr src_addr_grp,
		const DstZonePtr dst_zone,
		const DstAddressGroupPtr dst_addr_grp,
		const ServiceGroupPtr svc_grp,
		const ApplicationGroupPtr app_grp,
		const UserGroupPtr usr_grp,
		const UrlGroupPtr url_grp

	) const
	{
		Predicate::BddOptions bdd_options;

		if (src_zone)
			bdd_options.add(Predicate::BddOption::SourceZone);
		if (dst_zone)
			bdd_options.add(Predicate::BddOption::DestinationZone);
		if (app_grp)
			bdd_options.add(Predicate::BddOption::Application);
		if (usr_grp)
			bdd_options.add(Predicate::BddOption::User);
		if (url_grp)
			bdd_options.add(Predicate::BddOption::Url);

		// Prepare source definitions
		Sources sources{
			std::make_shared<SrcZoneGroup>("", src_zone),
			src_addr_grp, 
			false
		};

		// Prepare destination definitions
		Destinations destinations{
			std::make_shared<DstZoneGroup>("", dst_zone),
			dst_addr_grp,
			false
		};

		// Create the test predicate
		Predicate test_predicate{
			sources,
			destinations,
			svc_grp,
			app_grp,
			usr_grp,
			url_grp
		};

		Bddnode test_bdd{ test_predicate.make_bdd(bdd_options) };

		for (const auto& rule : _acl) {
			// Is the traffic defined by the test predicate accepted by this rule ?
			if (test_bdd.is_subset(Bddnode(rule->predicate().make_bdd(bdd_options)))) {
				return std::make_pair(rule->action() == RuleAction::ALLOW, rule);
			}
		}

		// Traffic is denied
		return std::make_pair(false, nullptr);
	}

}
