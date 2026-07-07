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
				std::make_shared<AnyServiceGroup>(),
				std::make_shared<AnyApplicationGroup>(),
				std::make_shared<AnyUserGroup>(),
				std::make_shared<AnyUrlGroup>()
			)
		{}

		bdd make_bdd() const override
		{
			return bddtrue;
		}

		static Sources any_sources() {
			return Sources{ std::make_shared<SrcAnyZoneGroup>(), std::make_shared<SrcAny4AddressGroup>(), false };
		}

		static Destinations any_destinations() {
			return Destinations{ std::make_shared<DstAnyZoneGroup>(), std::make_shared <DstAny4AddressGroup>(), false};
		}
	};


	class Any6Predicate final : public Predicate
	{
	public:
		Any6Predicate() :
			Predicate(
				any_sources(),
				any_destinations(),
				std::make_shared<AnyServiceGroup>(),
				std::make_shared<AnyApplicationGroup>(),
				std::make_shared<AnyUserGroup>(),
				std::make_shared<AnyUrlGroup>()
			)
		{}

		bdd make_bdd() const override
		{
			return bddtrue;
		}

		static Sources any_sources() {
			return Sources{ std::make_shared<SrcAnyZoneGroup>(), std::make_shared<SrcAny6AddressGroup>(), false };
		}

		static Destinations any_destinations() {
			return Destinations{ std::make_shared<DstAnyZoneGroup>(), std::make_shared <DstAny6AddressGroup>(), false };
		}
	};


	class Any64Predicate final : public Predicate
	{
	public:
		Any64Predicate() :
			Predicate(
				any_sources(),
				any_destinations(),
				std::make_shared<AnyServiceGroup>(),
				std::make_shared<AnyApplicationGroup>(),
				std::make_shared<AnyUserGroup>(),
				std::make_shared<AnyUrlGroup>()
			)
		{}

		bdd make_bdd() const override
		{
			return bddtrue;
		}

		static Sources any_sources() {
			return Sources{ std::make_shared<SrcAnyZoneGroup>(), std::make_shared<SrcAny64AddressGroup>(), false };
		}

		static Destinations any_destinations() {
			return Destinations{ std::make_shared<DstAnyZoneGroup>(), std::make_shared <DstAny64AddressGroup>(), false };
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
			_predicate{predicate}
		{}

		SrcZoneGroupPtr create_src_zones()
		{
			// Compute symmetrical source zones.
			SrcZoneGroup* sym_src_zones{ new SrcZoneGroup("") };
			for (const DstZone* dz : _predicate.dst_zones().items())
				sym_src_zones->add_member(create(*dz));

			return SrcZoneGroupPtr(sym_src_zones);
		}

		SrcAddressGroupPtr create_src_addr()
		{
			// Compute symmetrical source addresses.
			SrcAddressGroup* sym_src_addr{ new SrcAddressGroup("") };
			for (const DstAddress* da : _predicate.dst_addresses().items())
				sym_src_addr->add_member(create(*da));

			return SrcAddressGroupPtr(sym_src_addr);
		}

		DstZoneGroupPtr create_dst_zones()
		{
			// Compute symmetrical destination zones.
			DstZoneGroup* sym_dst_zones{ new DstZoneGroup("") };
			for (const SrcZone* sz : _predicate.src_zones().items())
				sym_dst_zones->add_member(create(*sz));

			return DstZoneGroupPtr(sym_dst_zones);
		}

		DstAddressGroupPtr create_dst_addr()
		{
			// Compute symmetrical destination addresses.
			DstAddressGroup* sym_dst_addr{ new DstAddressGroup("") };
			for (const SrcAddress* sa : _predicate.src_addresses().items())
				sym_dst_addr->add_member(create(*sa));

			return DstAddressGroupPtr(sym_dst_addr);
		}

	private:
		const Predicate& _predicate;

		static SrcZonePtr create(const DstZone& zone)
		{
			return SrcZonePtr(new SrcZone(zone.name(), zone));
		}

		static DstZonePtr create(const SrcZone& zone)
		{
			return DstZonePtr(new DstZone(zone.name(), zone));
		}

		static SrcAddressPtr create(const DstAddress& addr)
		{
			return SrcAddressPtr(new SrcAddress(addr.name(), addr));
		}

		static DstAddressPtr create(const SrcAddress& addr)
		{
			return DstAddressPtr(new DstAddress(addr.name(), addr));
		}

	};


	Predicate::Predicate(
		const Sources& sources,
		const Destinations& destinations,
		ServiceGroupPtr services,
		ApplicationGroupPtr applications,
		UserGroupPtr users,
		UrlGroupPtr urls
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
		assert(_src_zones && _dst_zones && _src_addresses && _dst_addresses && _services &&
			_applications && _users && _urls);
	}


	Predicate::Predicate(const Predicate& other) :
		_src_zones{ other._src_zones },
		_dst_zones{ other._dst_zones },
		_src_addresses{ other._src_addresses },
		_negate_src_addresses{ other._negate_src_addresses },
		_dst_addresses{ other._dst_addresses },
		_negate_dst_addresses{ other._negate_dst_addresses },
		_services{ other._services },
		_applications{ other._applications },
		_users{ other._users },
		_urls{ other._urls }
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
		SymmetricalPredicateCreator creator(*this);

		return new Predicate(
			Sources{
				creator.create_src_zones(),
				creator.create_src_addr(),
				negate_dst_addresses()
			},
			Destinations{
				creator.create_dst_zones(),
				creator.create_dst_addr(),
				negate_src_addresses()
			},
			_services,
			_applications,
			_users,
			_urls
		);

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
