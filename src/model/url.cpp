/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#include "model/url.h"

#include <memory>

#include "model/domains.h"
#include "model/moptions.h"

namespace fwm {


	bool is_url(const std::string& url)
	{
		return true;
	}


	Url::Url(const std::string& url, Range* range, const ModelOptions& options) :
		NamedMnode(url, options),
		_url_value{ std::make_unique<Mvalue>(DomainType::Url, range) }
	{
	}


	Url::Url(const std::string& url, DomainType dt, uint16_t url_id, const ModelOptions& options) :
		Url(url, UrlDomain::create_singleton(url_id), options)
	{
	}


	Url::Url(const Url& url) :
		Url(url.name(), url.value().range().clone(), url.options())
	{
	}


	Url* Url::create(const std::string& url, uint16_t url_id, const ModelOptions& options)
	{
		if (url == "any")
			return any();
		else
			return new Url(url, DomainType::Url, url_id, options);
	}


	Url* Url::any()
	{
		return new Url("any", UrlDomain::create_full_range(), ModelOptions::empty());
	}


	bdd Url::make_bdd() const
	{
		return options().contains(ModelOption::Url)
			? _url_value->make_bdd()
			: bddtrue;
	}


	const Mvalue& Url::value() const
	{
		return *_url_value;
	}


	std::string Url::to_string() const
	{
		return _url_value->to_string();
	}


	AnyUrlGroup::AnyUrlGroup() :
		UrlGroup("$any-url-group", Url::any())
	{
	}


	AnyUrlGroup::~AnyUrlGroup()
	{
		// delete the "any" Url allocated in the constructor
		parse([](const Url* url){ delete url; });
	}


	UrlGroup* AnyUrlGroup::clone() const
	{
		return new AnyUrlGroup();
	}


	bdd AnyUrlGroup::make_bdd() const
	{
		return bddtrue;
	}

}
