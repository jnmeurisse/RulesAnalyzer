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


	class SymmetricalPredicateCreator {
		// It is not possible to compare a source address with a destination address for
		// equality (or any other comparator).  (It is the same for a source zone and
		// destination zone).  The domain used to encode a source address is different
		// from the domain used for a destination address.  A comparison between the
		// source bdd and the destination bdd will always result to false (except for any).
		// For this reason, we are creating a temporary new source (addresses, zones) for
		// each destination and vis versa.  All temporary sources and destinations are
		// referenced in list of unique pointers to guarantee their destruction.
	public:
		SymmetricalPredicateCreator(const Predicate& predicate) :
			_predicate{predicate},
			_src_zones{},
			_dst_zones{},
			_src_addresses{},
			_dst_addresses{}
		{}

		SrcZoneGroup* create_src_zones()
		{
			// Compute symmetrical source zones.
			SrcZoneGroup* sym_src_zones{ new SrcZoneGroup("") };
			for (const DstZone* dz : _predicate.dst_zones().items())
				sym_src_zones->add_member(create(*dz));

			return sym_src_zones;
		}

		SrcAddressGroup* create_src_addr()
		{
			// Compute symmetrical source addresses.
			SrcAddressGroup* sym_src_addr{ new SrcAddressGroup("") };
			for (const DstAddress* da : _predicate.dst_addresses().items())
				sym_src_addr->add_member(create(*da));

			return sym_src_addr;
		}

		DstZoneGroup* create_dst_zones()
		{
			// Compute symmetrical destination zones.
			DstZoneGroup* sym_dst_zones{ new DstZoneGroup("") };
			for (const SrcZone* sz : _predicate.src_zones().items())
				sym_dst_zones->add_member(create(*sz));

			return sym_dst_zones;
		}

		DstAddressGroup* create_dst_addr()
		{
			// Compute symmetrical destination addresses.
			DstAddressGroup* sym_dst_addr{ new DstAddressGroup("") };
			for (const SrcAddress* sa : _predicate.src_addresses().items())
				sym_dst_addr->add_member(create(*sa));

			return sym_dst_addr;
		}

		ServiceGroup* create_services() const
		{
			return _predicate.services().clone();
		}

		ApplicationGroup* create_applications() const
		{
			return _predicate.applications().clone();
		}

		UserGroup* create_users() const
		{
			return _predicate.users().clone();
		}

		UrlGroup* create_urls() const
		{
			return _predicate.urls().clone();
		}

		inline const Predicate& predicate() const {return _predicate; }

	private:
		const Predicate& _predicate;
		std::list<std::unique_ptr<SrcZone>> _src_zones;
		std::list<std::unique_ptr<DstZone>> _dst_zones;
		std::list<std::unique_ptr<SrcAddress>> _src_addresses;
		std::list<std::unique_ptr<DstAddress>> _dst_addresses;

		const SrcZone* create(const DstZone& zone)
		{
			_src_zones.push_back(std::make_unique<SrcZone>("", zone));
			return _src_zones.back().get();
		}

		const DstZone* create(const SrcZone& zone)
		{
			_dst_zones.push_back(std::make_unique<DstZone>(zone.name(), zone));
			return _dst_zones.back().get();
		}

		const SrcAddress* create(const DstAddress& addr)
		{
			_src_addresses.push_back(std::make_unique<SrcAddress>(addr.name(), addr));
			return _src_addresses.back().get();
		}

		const DstAddress* create(const SrcAddress& addr)
		{
			_dst_addresses.push_back(std::make_unique<DstAddress>(addr.name(), addr));
			return _dst_addresses.back().get();
		}

	};


	class SymmetricalPredicate final : public Predicate
	{
	public:
		SymmetricalPredicate(SymmetricalPredicateCreator* creator) :
			_creator{creator},
			Predicate(
				Sources{
					creator->create_src_zones(),
					creator->create_src_addr(),
					creator->predicate().negate_src_addresses()
				},
				Destinations{
					creator->create_dst_zones(),
					creator->create_dst_addr(),
					creator->predicate().negate_dst_addresses()
				},
				creator->create_services(),
				creator->create_applications(),
				creator->create_users(),
				creator->create_urls()
			)
		{
		}

	private:
		std::unique_ptr<SymmetricalPredicateCreator> _creator;

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
		_dst_addresses{ destinations.dst_addresses },
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


	bdd Predicate::src_addresses_bdd() const
	{
		return _src_addresses->negate_if(_negate_src_addresses).make_bdd();
	}


	bdd Predicate::dst_addresses_bdd() const
	{
		return _dst_addresses->negate_if(_negate_dst_addresses).make_bdd();
	}


	Predicate* Predicate::symmetrical() const
	{
		return new SymmetricalPredicate(new SymmetricalPredicateCreator(*this));
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
