/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#include <list>
#include <memory>
#include <utility>

#include "bdd.h"
#include "address.h"
#include "domains.h"
#include "predicate.h"

namespace fwm {

	class AnyPredicate : public Predicate
	{
	public:
		AnyPredicate() :
			Predicate(
				Sources {
					std::make_unique<SrcZoneGroup>("", SrcZone::any()),
					std::make_unique<SrcAddressGroup>("", SrcAddress::any())
				},
				Destinations{
					std::make_unique<DstZoneGroup>("", DstZone::any()),
					std::make_unique<DstAddressGroup>("", DstAddress::any())
				},
				std::make_unique<ServiceGroup>("", Service::any()),
				std::make_unique<ApplicationGroup>("", Application::any()),
				std::make_unique<UserGroup>("", User::any())
			)
		{}

		~AnyPredicate()
		{
			delete src_zones().items().front();
			delete dst_zones().items().front();
			delete src_addresses().items().front();
			delete dst_addresses().items().front();
			delete services().items().front();
			delete applications().items().front();
			delete users().items().front();
		}

		bdd make_bdd() const override
		{
			return bddtrue;
		}
	};


	Predicate::Predicate(
		Sources& sources,
		Destinations& destinations,
		ServiceGroupPtr& services,
		ApplicationGroupPtr& applications,
		UserGroupPtr& users
	) :
		_src_zones{ std::move(sources.src_zones) },
		_dst_zones{ std::move(destinations.dst_zones) },
		_src_addresses{ std::move(sources.src_addresses) },
		_negate_src_addresses{ sources.negate_src_addresses },
		_dst_addresses{ std::move(destinations.dst_addresses) },
		_negate_dst_addresses{ destinations.negate_dst_addresses },
		_services{ std::move(services) },
		_applications{ std::move(applications) },
		_users{ std::move(users) }
	{
	}


	Predicate::Predicate(const Predicate& other) :
		_src_zones{ new SrcZoneGroup(*other._src_zones) },
		_dst_zones{ new DstZoneGroup(*other._dst_zones) },
		_src_addresses{ new SrcAddressGroup(*other._src_addresses) },
		_negate_src_addresses{ other._negate_src_addresses },
		_dst_addresses{ new DstAddressGroup(*other._dst_addresses) },
		_negate_dst_addresses{ other._negate_dst_addresses },
		_services{ new ServiceGroup(*other._services) },
		_applications{ new ApplicationGroup(*other._applications) },
		_users{ new UserGroup(*other._users) }
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
			_users->make_bdd();
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

		return output_bdd;
	}


	bool Predicate::is_symmetrical(const Predicate& other, bool strict) const noexcept
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
			const Interval& interval{ dz->interval() };
			src_zones.push_back(std::make_unique<SrcZone>(no_name, interval.lower(), interval.upper()));
			other_symsrc_zones->add_member(src_zones.back().get());
		}

		// Compute symmetrical destination zones.
		std::list<std::unique_ptr<DstZone>> dst_zones;
		DstZoneGroupPtr other_symdst_zones{ std::make_unique<DstZoneGroup>("") };
		for (const SrcZone* sz : other.src_zones().items()) {
			const Interval& interval{ sz->interval() };
			dst_zones.push_back(std::make_unique<DstZone>(no_name, interval.lower(), interval.upper()));
			other_symdst_zones->add_member(dst_zones.back().get());
		}

		// Compute symmetrical source addresses.
		std::list<std::unique_ptr<SrcAddress>> src_addresses;
		SrcAddressGroupPtr other_symsrc_addresses{ std::make_unique<SrcAddressGroup>("") };
		for (const DstAddress* da : other.dst_addresses().items()) {
			const Interval& interval{ da->interval() };
			src_addresses.push_back(std::make_unique<SrcAddress>(no_name, da->at(), interval.lower(), interval.upper()));
			other_symsrc_addresses->add_member(src_addresses.back().get());
		}

		// Negate symmetrical source addresses if requested.
		const Bddnode other_symsrc_addr_bdd = other_symsrc_addresses->negate_if(other._negate_src_addresses);

		// Compute symmetrical destination addresses.
		std::list<std::unique_ptr<DstAddress>> dst_addresses;
		DstAddressGroupPtr other_symdst_addresses{ std::make_unique<DstAddressGroup>("") };
		for (const SrcAddress* sa : other.src_addresses().items()) {
			const Interval& interval{ sa->interval() };
			dst_addresses.push_back(std::make_unique<DstAddress>(no_name, sa->at(), interval.lower(), interval.upper()));
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

		return symmetrical;
	}


	Predicate* Predicate::any()
	{
		return new AnyPredicate();
	};

}
