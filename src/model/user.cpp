/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#include "model/user.h"

#include <memory>

#include "model/domains.h"
#include "model/moptions.h"

namespace fwm {

	User::User(const std::string& name, Range* range, const ModelOptions& options) :
		NamedMnode(name, options),
		_user_value{ std::make_unique<Mvalue>(DomainType::User, range) }
	{
	}


	User::User(const std::string& name, DomainType dt, uint16_t user_id, const ModelOptions& options) :
		User(name, UserDomain::create_singleton(user_id), options)
	{
	}

	User::User(const User& user) :
		User(user.name(), user.value().range().clone(), user.options())
	{
	}


	User* User::create(const std::string& name, uint16_t user_id, const ModelOptions& options)
	{
		return new User(name, DomainType::User, user_id, options);
	}


	User* User::any()
	{
		return new User("any", UserDomain::create_full_range(), ModelOptions::empty());
	}


	bdd User::make_bdd() const
	{
		return options().contains(ModelOption::User)
			? _user_value->make_bdd()
			: bddtrue;
	}


	const Mvalue& User::value() const
	{
		return *_user_value;
	}


	std::string User::to_string() const
	{
		return _user_value->to_string();
	}


	AnyUserGroup::AnyUserGroup() :
		UserGroup("$any-usr-group", User::any())
	{
	}


	AnyUserGroup::~AnyUserGroup()
	{
		// delete the "any" user allocated in the constructor
		parse([](const User* user){ delete user; });
	}


	UserGroup* AnyUserGroup::clone() const
	{
		return new AnyUserGroup();
	}


	bdd AnyUserGroup::make_bdd() const
	{
		return bddtrue;
	}

}
