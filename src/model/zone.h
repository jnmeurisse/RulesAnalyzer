/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#pragma once
#include <list>
#include <memory>
#include <string>

#include "mnode.h"
#include "domain.h"
#include "group.h"
#include "interval.h"
#include "table.h"


namespace fwm {

	class Zone : public NamedMnode
	{
	public:
		/* Creates a binary decision diagram from this zone.
		*/
		virtual bdd make_bdd() const override;

		/* returns the zone range.
		*/
		const Interval& interval() const;

		/* returns the zone value as a string.
		*/
		virtual std::string to_string() const override;

	protected:
		Zone(const std::string& name, DomainType dt, uint32_t lower, uint32_t upper);

	private:
		const Interval _zone;
	};


	class SrcZone : public Zone
	{
	public:
		SrcZone(const std::string& name, uint32_t lower, uint32_t upper);

		static SrcZone* create(const std::string& name, uint32_t zone);
		static SrcZone* any();
	};


	class DstZone : public Zone
	{
	public:
		DstZone(const std::string& name, uint32_t lower, uint32_t upper);

		static DstZone* create(const std::string& name, uint32_t zone_id);
		static DstZone* any();
	};


	class SrcZoneGroup final : public Group<SrcZone>
	{
	public:
		SrcZoneGroup() = delete;

		SrcZoneGroup(const std::string& name);
		SrcZoneGroup(const std::string& name, const SrcZone* zone);
	};

	using SrcZoneGroupPtr = std::unique_ptr<SrcZoneGroup>;


	class DstZoneGroup final : public Group<DstZone>
	{
	public:
		DstZoneGroup() = delete;

		DstZoneGroup(const std::string& name);
		DstZoneGroup(const std::string& name, const DstZone* zone);
	};

	using DstZoneGroupPtr = std::unique_ptr<DstZoneGroup>;


	/* A list of zone names
	*/
	class ZoneNames : public std::list<std::string>
	{
	public:
		/* Moves or add a zone name at the end of this list.
		*/
		ZoneNames& move_at_end(const std::string& zone_name);

		/* Removes a zone name from this list.
		*/
		ZoneNames& remove(const std::string& zone_name);

		/* Sorts and Remove all duplicate names from the list.
		*/
		ZoneNames& unique();
	};


	/* A source and destination zones.
	*/
	struct ZonePair {
		const SrcZone* src_zone;
		const DstZone* dst_zone;
	};

}
