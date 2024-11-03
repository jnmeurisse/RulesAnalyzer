/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#include "zone.h"

#include <algorithm>
#include <memory>

#include "model/domain.h"
#include "model/domains.h"
#include "model/moptions.h"


namespace fwm {

	Zone::Zone(const std::string& name, Mvalue* value) :
		NamedMnode(name, ModelOptions::empty()),
		_zone_value{ value }
	{
	}


	bdd Zone::make_bdd() const
	{
		return _zone_value->make_bdd();
	}


	const Mvalue& Zone::value() const
	{
		return *_zone_value;
	}


	std::string Zone::to_string() const
	{
		return _zone_value->to_string();
	}


	SrcZone::SrcZone(const std::string& name, Range* range) :
		Zone(name, new Mvalue(DomainType::SrcZone, range))
	{
	}


	SrcZone::SrcZone(const std::string &name, uint16_t zone_id) :
		SrcZone(name, SrcZoneDomain::create_singleton(zone_id))
	{
	}


	SrcZone::SrcZone(const std::string& name, const Zone& zone) :
		SrcZone(name, zone.value().range().clone())
	{
	}


	SrcZone::SrcZone(const SrcZone& zone) :
		SrcZone(zone.name(), zone)
	{
	}


	SrcZone* SrcZone::create(const std::string& name, uint16_t zone_id)
	{
		return new SrcZone(name, zone_id);
	}


	SrcZone* SrcZone::any()
	{
		return new SrcZone("any", SrcZoneDomain::create_full_range());
	}


	SrcAnyZoneGroup::SrcAnyZoneGroup() :
		SrcZoneGroup("$src-any-zone-group", SrcZone::any())
	{
	}


	SrcAnyZoneGroup::~SrcAnyZoneGroup()
	{
		// delete the "any" zone allocated in the constructor
		parse([](const SrcZone* zone){ delete zone; });
	}


	SrcZoneGroup* SrcAnyZoneGroup::clone() const
	{
		return new SrcAnyZoneGroup();
	}


	bdd SrcAnyZoneGroup::make_bdd() const
	{
		return bddtrue;
	}


	DstZone::DstZone(const std::string& name, Range* range) :
		Zone(name, new Mvalue(DomainType::DstZone, range))
	{
	}


	DstZone::DstZone(const std::string& name, uint16_t zone_id) :
		DstZone(name, DstZoneDomain::create_singleton(zone_id))
	{
	}


	DstZone::DstZone(const std::string& name, const Zone& zone) :
		DstZone(name, zone.value().range().clone())
	{
	}


	DstZone::DstZone(const DstZone& zone) :
		DstZone(zone.name(), zone)
	{
	}


	DstZone* DstZone::create(const std::string& name, uint16_t zone_id)
	{
		return new DstZone(name, zone_id);
	}


	DstZone* DstZone::any()
	{
		return new DstZone("any", DstZoneDomain::create_full_range());
	}


	DstAnyZoneGroup::DstAnyZoneGroup() :
		DstZoneGroup("$dst-any-zone-group", DstZone::any())
	{
	}


	DstAnyZoneGroup::~DstAnyZoneGroup()
	{
		// delete the "any" zone allocated in the constructor
		parse([](const DstZone* zone){ delete zone; });
	}


	DstZoneGroup* DstAnyZoneGroup::clone() const
	{
		return new DstAnyZoneGroup();
	}


	bdd DstAnyZoneGroup::make_bdd() const
	{
		return bddtrue;
	}

}
