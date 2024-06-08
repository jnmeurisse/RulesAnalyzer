/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#pragma once
#include <memory>
#include <string>

#include "mnode.h"
#include "domain.h"
#include "group.h"
#include "interval.h"
#include "moptions.h"
#include "table.h"

namespace fwm {

	class User : public NamedMnode
	{
	public:
		User(const std::string& name, DomainType dt, uint32_t lower, uint32_t upper, const ModelOptions& options);

		static User* create(const std::string& name, uint32_t user_id, const ModelOptions& options);
		static User* any();

		/* Creates a binary decision diagram for this user.
		*/
		virtual bdd make_bdd() const override;

		/* returns the user value as a string.
		*/
		virtual std::string to_string() const override;

	private:
		const Interval _user;
	};


	class UserGroup : public Group<User>
	{
	public:
		UserGroup() = delete;
		UserGroup(const std::string& name);
		UserGroup(const std::string& name, const User* user);
	};

	using UserGroupPtr = std::unique_ptr<UserGroup>;

}
