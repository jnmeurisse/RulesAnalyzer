/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#include "model/predicate.h"

#include <list>
#include <memory>
#include <utility>
#include <buddy/bdd.h>

#include "model/address.h"
#include "model/domains.h"


namespace fwm {

	class Any4Predicate final : public Predicate
	{
	public:
		Any4Predicate() :
			Predicate(
				any_sources(),
				any_destinations(),
				new AnyServiceGroup(),
				new AnyApplicationGroup(),
				new AnyUserGroup(),
				new AnyUrlGroup()
			)
		{}

		bdd make_bdd() const override
		{
			return bddtrue;
		}

		static Sources any_sources() {
			return Sources{ new SrcAnyZoneGroup(), new SrcAny4AddressGroup(), false };
		}

		static Destinations any_destinations() {
			return Destinations{ new DstAnyZoneGroup(), new DstAny4AddressGroup, false };
		}
	};


	class Any6Predicate final : public Predicate
	{
	public:
		Any6Predicate() :
			Predicate(
				any_sources(),
				any_destinations(),
				new AnyServiceGroup(),
				new AnyApplicationGroup(),
				new AnyUserGroup(),
				new AnyUrlGroup()
			)
		{}

		bdd make_bdd() const override
		{
			return bddtrue;
		}

		static Sources any_sources() {
			return Sources{ new SrcAnyZoneGroup(), new SrcAny6AddressGroup(), false };
		}

		static Destinations any_destinations() {
			return Destinations{ new DstAnyZoneGroup(), new DstAny6AddressGroup(), false };
		}
	};


	class Any64Predicate final : public Predicate
	{
	public:
		Any64Predicate() :
			Predicate(
				any_sources(),
				any_destinations(),
				new AnyServiceGroup(),
				new AnyApplicationGroup(),
				new AnyUserGroup(),
				new AnyUrlGroup()
			)
		{}

		bdd make_bdd() const override
		{
			return bddtrue;
		}

		static Sources any_sources() {
			return Sources{ new SrcAnyZoneGroup(), new SrcAny64AddressGroup(), false };
		}

		static Destinations any_destinations() {
			return Destinations{ new DstAnyZoneGroup(), new DstAny64AddressGroup(), false };
		}
	};


	Predicate::Predicate(
		const Sources& sources,
		const Destinations& destinations,
		ServiceGroup* services,
		ApplicationGroup* applications,
		UserGroup* users,
		UrlGroup* urls
	) :
		_src_zones{ sources.src_zones },
		_dst_zones{ destinations.dst_zones },
		_src_addresses{ sources.src_addresses },
		_negate_src_addresses{ sources.negate_src_addresses },
		_dst_addresses{ std::move(destinations.dst_addresses) },
		_negate_dst_addresses{ destinations.negate_dst_addresses },
		_services{ services },
		_applications{ applications },
		_users{ users },
		_urls{ urls }
	{
	}


	Predicate::Predicate(const Predicate& other) :
		_src_zones{ other._src_zones->clone() },
		_dst_zones{ other._dst_zones->clone() },
		_src_addresses{ other._src_addresses->clone() },
		_negate_src_addresses{ other._negate_src_addresses },
		_dst_addresses{ other._dst_addresses->clone() },
		_negate_dst_addresses{ other._negate_dst_addresses },
		_services{ other._services->clone() },
		_applications{ other._applications->clone() },
		_users{ other._users->clone() },
		_urls{ other._urls->clone() }
	{
	}


	bdd Predicate::make_bdd() const
	{
		return
			_src_zones->make_bdd() &
			_dst_zones->make_bdd() &
			_src_addresses->negate_if(_negate_src_addresses).make_bdd() &
			_dst_addresses->negate_if(_negate_dst_addresses).make_bdd() &
			(_services->is_app_services() ? bddtrue : _services->make_bdd()) &
			_applications->make_bdd() &
			_users->make_bdd() &
			_urls->make_bdd();
	}


	bdd Predicate::make_bdd(BddOptions options) const
	{
		bdd output_bdd =
			_src_addresses->negate_if(_negate_src_addresses).make_bdd() &
			_dst_addresses->negate_if(_negate_dst_addresses).make_bdd();

		if (options.contains(BddOption::SourceZone))
			output_bdd &= _src_zones->make_bdd();
		if (options.contains(BddOption::DestinationZone))
			output_bdd &= _dst_zones->make_bdd();
		if (options.contains(BddOption::Application))
			output_bdd &=
				(_services->is_app_services() ? bddtrue : _services->make_bdd())
				& _applications->make_bdd();
		else
			output_bdd &= _services->is_app_services()
				? _applications->default_services()->make_bdd()
				: _services->make_bdd();
		if (options.contains(BddOption::User))
			output_bdd &= _users->make_bdd();
		if (options.contains(BddOption::Url))
			output_bdd &= _urls->make_bdd();

		return output_bdd;
	}


	bool Predicate::is_symmetrical(const Predicate& other, bool strict) const
	{
		const std::string no_name;
		bool symmetrical{ false };

		// It is not possible to compare a source address with a destination address for
		// equality (or any other comparator).  (It is the same for a source zone and
		// destination zone).  The domain used to encode a source address is different
		// from the domain used for a destination address.  A comparison between the
		// source bdd and the destination bdd will always result to false (except for any).
		// For this reason, we are creating a temporary new source (addresses, zones) for
		// each destination and vis versa.  All temporary sources and destinations are
		// referenced in list of unique pointers to guarantee their destruction at the
		// exit of this method.

		// Compute symmetrical source zones.
		std::list<std::unique_ptr<SrcZone>> src_zones;
		SrcZoneGroupPtr other_symsrc_zones{ std::make_unique<SrcZoneGroup>("") };
		for (const DstZone* dz : other.dst_zones().items()) {
			src_zones.push_back(std::make_unique<SrcZone>(no_name, *dz));
			other_symsrc_zones->add_member(src_zones.back().get());
		}

		// Compute symmetrical destination zones.
		std::list<std::unique_ptr<DstZone>> dst_zones;
		DstZoneGroupPtr other_symdst_zones{ std::make_unique<DstZoneGroup>("") };
		for (const SrcZone* sz : other.src_zones().items()) {
			dst_zones.push_back(std::make_unique<DstZone>(no_name, *sz));
			other_symdst_zones->add_member(dst_zones.back().get());
		}

		// Compute symmetrical source addresses.
		std::list<std::unique_ptr<SrcAddress>> src_addresses;
		SrcAddressGroupPtr other_symsrc_addresses{ std::make_unique<SrcAddressGroup>("") };
		for (const DstAddress* da : other.dst_addresses().items()) {
			src_addresses.push_back(std::make_unique<SrcAddress>(no_name, *da));
			other_symsrc_addresses->add_member(src_addresses.back().get());
		}

		// Negate symmetrical source addresses if requested.
		const Bddnode other_symsrc_addr_bdd = other_symsrc_addresses->negate_if(other._negate_src_addresses);

		// Compute symmetrical destination addresses.
		std::list<std::unique_ptr<DstAddress>> dst_addresses;
		DstAddressGroupPtr other_symdst_addresses{ std::make_unique<DstAddressGroup>("") };
		for (const SrcAddress* sa : other.src_addresses().items()) {
			dst_addresses.push_back(std::make_unique<DstAddress>(no_name, *sa));
			other_symdst_addresses->add_member(dst_addresses.back().get());
		}

		// Negate symmetrical destination addresses if requested.
		const Bddnode other_symdst_addr_bdd = other_symdst_addresses->negate_if(other._negate_dst_addresses);

		// negate source addresses if requested.
		const Bddnode src_addr_bdd = _src_addresses->negate_if(_negate_src_addresses);

		// negate destination addresses if requested.
		const Bddnode dst_addr_bdd = _dst_addresses->negate_if(_negate_dst_addresses);

		if (strict) {
			// Equality comparison
			symmetrical =
				other_symsrc_zones->equal(*_src_zones) &&
				other_symdst_zones->equal(*_dst_zones) &&
				other_symsrc_addr_bdd.equal(src_addr_bdd) &&
				other_symdst_addr_bdd.equal(dst_addr_bdd) &&
				other.services().equal(*_services);
		}
		else
		{
			// Subset comparison
			symmetrical =
				other_symsrc_zones->is_subset(*_src_zones) &&
				other_symdst_zones->is_subset(*_dst_zones) &&
				other_symsrc_addr_bdd.is_subset(src_addr_bdd) &&
				other_symdst_addr_bdd.is_subset(dst_addr_bdd) &&
				other.services().is_subset(*_services);
		}

		if (symmetrical && _applications->options().contains(ModelOption::Application))
			symmetrical = _applications->equal(other.applications());

		if (symmetrical && _applications->options().contains(ModelOption::User))
			symmetrical = _users->equal(other.users());

		if (symmetrical && _urls->options().contains(ModelOption::Url))
			symmetrical = _urls->equal(other.urls());

		return symmetrical;
	}


	Predicate* Predicate::any(IPAddressModel ip_model)
	{
		switch (ip_model) {
		case IPAddressModel::IP4Model:
			return new Any4Predicate();

		case IPAddressModel::IP6Model:
			return new Any6Predicate();

		default:
			return new Any64Predicate();
		}
	};

}
