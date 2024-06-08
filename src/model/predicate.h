/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#pragma once
#include "address.h"
#include "application.h"
#include "mnode.h"
#include "service.h"
#include "user.h"
#include "zone.h"

#include "tools/options.h"


namespace fwm {
	struct Sources {
		SrcZoneGroupPtr& src_zones;
		SrcAddressGroupPtr& src_addresses;
		bool negate_src_addresses;
	};


	struct Destinations {
		DstZoneGroupPtr& dst_zones;
		DstAddressGroupPtr& dst_addresses;
		bool negate_dst_addresses;
	};


	class Predicate : public Mnode
	{
	public:
		enum class BddOption {
			SourceZone,
			DestinationZone,
			Application,
			User
		};

		class BddOptions : public Options<BddOption>
		{
		public:
			using Options<BddOption>::Options;
		};


	public:
		Predicate(
			Sources& sources,
			Destinations& destinations,
			ServiceGroupPtr& services,
			ApplicationGroupPtr& applications,
			UserGroupPtr& users
			);
		Predicate(const Predicate& other);

		static Predicate* any();

		/* Creates a binary decision diagram for this predicate.  The bdd combines
		*  source and destination zones, source and destination addresses, services
		*  (evaluated to bddtrue if application default service is enabled),
		*  applications (and default services) and users.
		*/
		virtual bdd make_bdd() const override;

		/* Creates a binary decision diagram for this predicate.  The bdd combines
		*  criteria selected
		*/
		bdd make_bdd(BddOptions options) const;

		/* Returns all source zones configured on a rule.
		*/
		inline const SrcZoneGroup& src_zones() const noexcept { return *_src_zones; }

		/* Returns all destination zones configured on a rule.
		*/
		inline const DstZoneGroup& dst_zones() const noexcept { return *_dst_zones; }

		/* Returns all source addresses configured on a rule.
		*/
		inline const SrcAddressGroup& src_addresses() const noexcept { return *_src_addresses; }

		/* Returns true if the source addresses are negated.
		*/
		inline bool negate_src_addresses() const noexcept { return _negate_src_addresses; }

		/* Returns all destination addresses configured on a rule.
		*/
		inline const DstAddressGroup& dst_addresses() const noexcept { return *_dst_addresses; }

		/* Returns true if the destination addresses are negated.
		*/
		inline bool negate_dst_addresses() const noexcept { return _negate_dst_addresses; }

		/* Returns all services configured on a rule.
		*/
		inline const ServiceGroup& services() const noexcept { return *_services; }

		/* Returns all applications configured on a rule.
		*/
		inline const ApplicationGroup& applications() const noexcept { return *_applications; }

		/* Returns all uses configured on a rule.
		*/
		inline const UserGroup& users() const noexcept { return *_users; }

		/* Returns true if two predicates are symmetrical.  Two predicates A, B are
		*  symmetrical when
		*       Predicate A             Predicate B
		*       destination zones     = source zones
		*       destination addresses = source addresses
		*       services              = services
		*       applications          = applications
		*       users                 = users
		*
		*  When strict is false, the method checks groups are a subset of each other
		*  instead of checking for equality.
		*/
		bool is_symmetrical(const Predicate& other, bool strict) const noexcept;

	private:
		const SrcZoneGroupPtr _src_zones;
		const DstZoneGroupPtr _dst_zones;
		const SrcAddressGroupPtr _src_addresses;
		const bool _negate_src_addresses;
		const DstAddressGroupPtr _dst_addresses;
		const bool _negate_dst_addresses;
		const ServiceGroupPtr _services;
		const ApplicationGroupPtr _applications;
		const UserGroupPtr _users;
	};

	using PredicatePtr = std::unique_ptr<const Predicate>;
}
