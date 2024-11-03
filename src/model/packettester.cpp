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
		const SrcZone* src_zone,
		const SrcAddressGroup& src_addr_grp,
		const DstZone* dst_zone,
		const DstAddressGroup& dst_addr_grp,
		const ServiceGroup& svc_grp,
		const ApplicationGroup* app_grp,
		const UserGroup* usr_grp,
		const UrlGroup* url_grp

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
			src_zone ? new SrcZoneGroup("", src_zone) : new SrcAnyZoneGroup(),
			src_addr_grp.clone(),
			false
		};

		// Prepare destination definitions
		Destinations destinations{
			dst_zone ? new DstZoneGroup("", dst_zone) : new DstAnyZoneGroup(),
			dst_addr_grp.clone(),
			false
		};

		// Prepare the services
		ServiceGroup* services{ svc_grp.clone() };

		// Prepare the applications
		ApplicationGroup* applications{ app_grp
			? app_grp->clone()
			: new AnyApplicationGroup()
		};

		// Prepare the users
		UserGroup* users{ usr_grp
			? usr_grp->clone()
			: new AnyUserGroup()
		};


		// Prepare the urls
		UrlGroup* urls{ url_grp
			? url_grp->clone()
			: new AnyUrlGroup()
		};

		// Create the test predicate
		Predicate test_predicate{
			sources,
			destinations,
			services,
			applications,
			users,
			urls
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
