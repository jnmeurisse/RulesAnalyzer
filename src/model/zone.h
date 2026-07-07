/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#pragma once
#include "global.h"

#include <cstdint>
#include <list>
#include <memory>
#include <string>

#include "model/mnode.h"
#include "model/domain.h"
#include "model/group.h"
#include "model/mvalue.h"


namespace fwm {

	class Zone abstract : public NamedMnode
	{
	public:
		/**
		 * Creates a binary decision diagram from this zone.
		 *
		 * @return a binary decision diagram.
		*/
		virtual bdd make_bdd() const override;

		/**
		 * Returns the zone value.
		*/
		const Mvalue& value() const;

		/**
		 * Returns the zone value as a string.
		*/
		virtual std::string to_string() const override;

	protected:
		/**
		 * Allocates a new zone.
		 *
		 * @param name The name of the zone.
		 * @param value The zone value.  The value must be dynamically allocated.
		 *              The Mvalue object is owned by this class.
		 *
		*/
		Zone(const std::string& name, Mvalue* value);

	private:
		// The zone value (a singleton or the whole domain).
		const MvaluePtr _zone_value;
	};


	/**
	 * A ZoneList represents a list of source and destination zones.
	*/
	using ZoneList = NamedMnodeList<Zone>;
	using ZoneListPtr = std::unique_ptr<ZoneList>;


	/**
	 * A SrcZone represents a source zone.
	*/
	class SrcZone final : public Zone
	{
	public:
		using Ptr = std::shared_ptr<const SrcZone>;

		/**
		 * Copy constructors.
		*/
		SrcZone(const std::string& name, const Zone& zone);
		SrcZone(const SrcZone& zone);

		/**
		 * Allocates a source zone
		 *
		 * @param name The name of the zone.
		 * @param zone_id The zone identifier.
		 *
		 * @return A source zone.
		*/
		static Ptr create(const std::string& name, uint16_t zone_id);

		/*
		 * Allocate a any source zone.
		 *
		 * @return A SrcZone representing all source zones.
		*/
		static Ptr any();

	private:
		/*
		 * Allocates a source zone from a range.
		*/
		SrcZone(const std::string& name, Range* range);

		/*
		 * Allocates a source zone.
		*/
		SrcZone(const std::string& name, uint16_t zone_id);
	};


	using SrcZonePtr = SrcZone::Ptr;


	/**
	 * A SrcZoneList represents a list of source.
	*/
	using SrcZoneList = NamedMnodeList<SrcZone>;
	using SrcZoneListPtr = std::unique_ptr<SrcZoneList>;


	/**
	 * SrcZoneGroup represents a hierarchy of groups of source zones.
	*/
	using SrcZoneGroup = Group<SrcZone>;
	using SrcZoneGroupPtr = std::shared_ptr<const SrcZoneGroup>;


	/**
	 * A SrcAnyZoneGroup is a group of source zones containing only one "any"
	 * source zone.
	 *
	*/
	class SrcAnyZoneGroup : public SrcZoneGroup
	{
	public:
		SrcAnyZoneGroup();

		/**
		 * Returns a bddtrue decision diagram.
		*/
		virtual bdd make_bdd() const override;
	};


	/**
	 * A DstZone represents a destination zone.
	*/
	class DstZone final : public Zone
	{
	public:
		using Ptr = std::shared_ptr<const DstZone>;

		/**
		 * Copy constructors.
		*/
		DstZone(const std::string& name, const Zone& zone);
		DstZone(const DstZone& zone);

		/**
		 * Allocates a destination zone
		 *
		 * @param name The name of the zone.
		 * @param zone_id The zone identifier.
		 *
		 * @return A destination zone.
		*/
		static Ptr create(const std::string& name, uint16_t zone_id);

		/**
		 * Allocates a any destination zone.
		 *
		 * @return A DstZone representing all destination zones.
		*/
		static Ptr any();

	protected:
		/**
		 * Allocates a destination zone from a range.
		*/
		DstZone(const std::string& name, Range* range);

		/**
		 * Allocates a destination zone.
		*/
		DstZone(const std::string& name, uint16_t zone_id);
	};


	using DstZonePtr = DstZone::Ptr;


	/**
	 * A DstZoneList represents a list of source.
	*/
	using DstZoneList = NamedMnodeList<DstZone>;
	using DstZoneListPtr = std::unique_ptr<DstZoneList>;


	/**
	 * DstZoneGroup represents a hierarchy of groups of destination zones.
	*/
	using DstZoneGroup = Group<DstZone>;
	using DstZoneGroupPtr = std::shared_ptr<const DstZoneGroup>;


	/**
	 * A DstAnyZoneGroup is a group of source zones containing only one "any"
	 * destination zone.
	 *
	*/
	class DstAnyZoneGroup : public DstZoneGroup
	{
	public:
		DstAnyZoneGroup();

		/**
		 * Returns a bddtrue decision diagram.
		*/
		virtual bdd make_bdd() const override;
	};


	/**
	 * A pair of source and destination zones.
	*/
	struct ZonePair {
		SrcZonePtr src_zone;
		DstZonePtr dst_zone;
	};

}
