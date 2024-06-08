/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#include "zone.h"

#include <algorithm>

#include "domains.h"
#include "moptions.h"


namespace fwm {
	Zone::Zone(const std::string& name, DomainType dt, uint32_t lower, uint32_t upper) :
		NamedMnode(name, ModelOptions::empty()),
		_zone{ dt, lower, upper }
	{
	}


	bdd Zone::make_bdd() const
	{
		return _zone.make_bdd();
	}


	const Interval& Zone::interval() const
	{
		return _zone;
	}


	std::string Zone::to_string() const
	{
		return _zone.to_string();
	}


	SrcZone::SrcZone(const std::string &name, uint32_t lower, uint32_t upper) :
		Zone(name, DomainType::SrcZone, lower, upper)
	{
	}


	SrcZone* SrcZone::create(const std::string& name, uint32_t zone)
	{
		return new SrcZone(name, zone, zone);
	}


	SrcZone* SrcZone::any()
	{
		return new SrcZone("any", 0, Domains::get(DomainType::SrcZone).upper());
	}


	DstZone::DstZone(const std::string& name, uint32_t lower, uint32_t upper) :
		Zone(name, DomainType::DstZone, lower, upper)
	{
	}


	DstZone* DstZone::create(const std::string& name, uint32_t zone_id)
	{
		return new DstZone(name, zone_id, zone_id);
	}


	DstZone* DstZone::any()
	{
		return new DstZone("any", 0, Domains::get(DomainType::DstZone).upper());
	}


	SrcZoneGroup::SrcZoneGroup(const std::string& name) :
		Group<SrcZone>(name)
	{
	}


	SrcZoneGroup::SrcZoneGroup(const std::string& name, const SrcZone* zone) :
		Group<SrcZone>(name, zone)
	{
	}


	DstZoneGroup::DstZoneGroup(const std::string& name) :
		Group<DstZone>(name)
	{
	}


	DstZoneGroup::DstZoneGroup(const std::string& name, const DstZone* zone) :
		Group<DstZone>(name, zone)
	{
	}


	ZoneNames& ZoneNames::move_at_end(const std::string& zone_name)
	{
		auto it = std::find(begin(), end(), zone_name);
		if (it != std::end(*this)) {
			splice(end(), *this, it);
		}
		else {
			push_back(zone_name);
		}

		return *this;
	}


	ZoneNames& ZoneNames::remove(const std::string& zone_name)
	{
		auto it = std::find(begin(), end(), zone_name);
		if (it != std::end(*this))
			this->erase(it);

		return *this;
	}

	ZoneNames& ZoneNames::unique()
	{
		sort();
		std::list<std::string>::unique();

		return *this;
	}

}
