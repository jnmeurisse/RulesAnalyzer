/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#pragma once
#include "global.h"

#include <memory>

#include "model/address.h"
#include "model/application.h"
#include "model/ipaddress.h"
#include "model/mnode.h"
#include "model/service.h"
#include "model/url.h"
#include "model/user.h"
#include "model/zone.h"
#include "tools/options.h"


namespace fwm {

	class Sources {
	public:
		SrcZoneGroup* src_zones;
		SrcAddressGroup* src_addresses;
		bool negate_src_addresses;
	};


	struct Destinations {
		DstZoneGroup* dst_zones;
		DstAddressGroup* dst_addresses;
		bool negate_dst_addresses;
	};


	class Predicate : public Mnode
	{
	public:
		enum class BddOption {
			SourceZone,
			DestinationZone,
			Application,
			User,
			Url
		};

		class BddOptions : public rat::Options<BddOption>
		{
		public:
			using Options<BddOption>::Options;
		};


	public:
		/**
		 * Allocates a predicate.
		 *
		 * The object becomes the owner of all objects.
		*/
		Predicate(
			const Sources& sources,
			const Destinations& destinations,
			ServiceGroup* services,
			ApplicationGroup* applications,
			UserGroup* users,
			UrlGroup* urls);

		/**
		 * Copy constructor.
		*/
		Predicate(const Predicate& other);

		/**
		 * Allocates a "any" predicate.
		 *
		 * @return A Predicate representing all source zones, address, destination
		 *         zones, adresses, services, applications and users.
		*/
		static Predicate* any(IPAddressModel ip_model);

		/**
		  * Creates a binary decision diagram for this predicate.
		  *
		  * The bdd combines source and destination zones, source and destination addresses,
		  * services (evaluated to bddtrue if application default service is enabled),
		  * applications (and default services) and users.
		*/
		virtual bdd make_bdd() const override;

		/**
		 * Creates a binary decision diagram for this predicate.
		 *
		 * The bdd combines the sources, destination etc.. according to the options.
		*/
		bdd make_bdd(BddOptions options) const;

		/**
		 * Returns a reference to all source zones configured on a rule.
		*/
		inline const SrcZoneGroup& src_zones() const noexcept { return *_src_zones; }

		/**
		 * Returns a reference to all destination zones configured on a rule.
		*/
		inline const DstZoneGroup& dst_zones() const noexcept { return *_dst_zones; }

		/**
		 * Returns a reference to all source addresses configured on a rule.
		*/
		inline const SrcAddressGroup& src_addresses() const noexcept { return *_src_addresses; }

		/**
		 * Returns true if the source addresses must be negated when computing a bdd.
		*/
		inline bool negate_src_addresses() const noexcept { return _negate_src_addresses; }

		/**
		 * Returns a reference to all destination addresses configured on a rule.
		*/
		inline const DstAddressGroup& dst_addresses() const noexcept { return *_dst_addresses; }

		/**
		 * Returns true if the destination addresses must be negated when computing a bdd.
		*/
		inline bool negate_dst_addresses() const noexcept { return _negate_dst_addresses; }

		/**
		 * Returns a reference to all services configured on a rule.
		*/
		inline const ServiceGroup& services() const noexcept { return *_services; }

		/**
		 * Returns a reference to all applications configured on a rule.
		*/
		inline const ApplicationGroup& applications() const noexcept { return *_applications; }

		/**
		 * Returns a reference to all uses configured on a rule.
		*/
		inline const UserGroup& users() const noexcept { return *_users; }

		/**
		 * Returns a reference to all urls configured on a rule.
		*/
		inline const UrlGroup& urls() const noexcept { return *_urls; }

		/* Returns true if two predicates are symmetrical.  Two predicates A, B are
		*  symmetrical when
		*       Predicate A             Predicate B
		*       destination zones     = source zones
		*       destination addresses = source addresses
		*       source zones          = destination zones
		*       source addresses      = destination addresses
		*       services              = services
		*       applications          = applications
		*       users                 = users
		*       urls                  = urls
		*
		*  When strict is false, the method checks if groups are a subset of each other
		*  instead of checking for equality.
		*/
		bool is_symmetrical(const Predicate& other, bool strict) const;

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
		const UrlGroupPtr _urls;
	};

	using PredicatePtr = std::unique_ptr<const Predicate>;
}
