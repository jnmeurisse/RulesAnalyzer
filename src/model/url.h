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
#include <memory>
#include <string>

#include "model/mnode.h"
#include "model/domain.h"
#include "model/group.h"
#include "model/moptions.h"
#include "model/mvalue.h"


namespace fwm {

	/* Returns true if the string is a valid url.
	*/
	bool is_url(const std::string& url);


	class Url final : public NamedMnode
	{
	public:
		/**
		 * Copy constructors.
		*/
		Url(const Url& url);

		/**
		 * Allocates a Url.
		 *
		 * @param url The url.
		 * @param url_id The Url identifier.
		 * @param options A reference to the model options.
		 *
		 * @return A source zone.
		*/
		static Url* create(const std::string& url, uint16_t url_id, const ModelOptions& options);

		/**
		 * Allocate a any Url.
		 *
		 * @return A Url representing all Urls.
		*/
		static Url* any();

		/**
		 * Creates a binary decision diagram from this url.
		 *
		 * @return a binary decision diagram.
		*/
		virtual bdd make_bdd() const override;

		/**
		 * Returns the Url value.
		*/
		const Mvalue& value() const;

		/**
		 * Returns the Url value as a string.
		*/
		virtual std::string to_string() const override;

	private:
		/*
		 * Allocates a Url from a range.
		*/
		Url(const std::string& url, Range* range, const ModelOptions& options);

		/*
		 * Allocates a Url.
		*/
		Url(const std::string& url, DomainType dt, uint16_t Url_id, const ModelOptions& options);

	private:
		// The Url value (a singleton or the whole domain).
		const MvaluePtr _url_value;
	};


	/**
	 * An UrlList represents a list of Urls.
	*/
	using UrlList = NamedMnodeList<Url>;
	using UrlListPtr = std::unique_ptr<UrlList>;


	/**
	 * UrlGroup represents a hierarchy of groups of Urls.
	*/
	using UrlGroup = Group<Url>;
	using UrlGroupPtr = std::unique_ptr<UrlGroup>;


	/*
	 * A AnyUrlGroup is a group of Urls containing only one "any" Url.
	 *
	*/
	class AnyUrlGroup : public UrlGroup
	{
	public:
		AnyUrlGroup();
		~AnyUrlGroup();

		/**
		 * Clones this group.
		*/
		virtual UrlGroup* clone() const override;

		/**
		 * Returns a bddtrue decision diagram.
		*/
		virtual bdd make_bdd() const override;
	};

}
