/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#include "user.h"
#include "domains.h"
#include "moptions.h"

namespace fwm {
	User::User(const std::string& name, DomainType dt, uint32_t lower, uint32_t upper, const ModelOptions& options) :
		NamedMnode(name, options),
		_user(dt, lower, upper)
	{
	}


	User* User::create(const std::string& name, uint32_t user_id, const ModelOptions& options)
	{
		return new User(name, DomainType::User, user_id, user_id, options);
	}


	User* User::any()
	{
		return new User(
			"any",
			DomainType::User,
			0,
			Domains::get(DomainType::User).upper(),
			ModelOptions::empty()
		);
	}


	bdd User::make_bdd() const
	{
		return options().contains(ModelOption::User)
			? _user.make_bdd()
			: bddtrue;
	}


	std::string User::to_string() const
	{
		return _user.to_string();
	}


	UserGroup::UserGroup(const std::string& name) :
		Group<User>(name)
	{
	}


	UserGroup::UserGroup(const std::string& name, const User* user) :
		Group<User>(name, user)
	{
	}

}
