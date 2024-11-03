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

	class User final : public NamedMnode
	{
	public:
		/**
		 * Copy constructors.
		*/
		User(const User& user);

		/**
		 * Allocates a user.
		 *
		 * @param name The name of the user.
		 * @param user_id The user identifier.
		 * @param options A reference to the model options.
		 *
		 * @return A source zone.
		*/
		static User* create(const std::string& name, uint16_t user_id, const ModelOptions& options);

		/**
		 * Allocate a any user.
		 *
		 * @return A User representing all users.
		*/
		static User* any();

		/**
		 * Creates a binary decision diagram from this user.
		 *
		 * @return a binary decision diagram.
		*/
		virtual bdd make_bdd() const override;

		/**
		 * Returns the user value.
		*/
		const Mvalue& value() const;

		/**
		 * Returns the user value as a string.
		*/
		virtual std::string to_string() const override;

	private:
		/*
		 * Allocates a user from a range.
		*/
		User(const std::string& name, Range* range, const ModelOptions& options);

		/*
		 * Allocates a user.
		*/
		User(const std::string& name, DomainType dt, uint16_t user_id, const ModelOptions& options);

	private:
		// The user value (a singleton or the whole domain).
		const MvaluePtr _user_value;
	};


	/**
	 * An UserList represents a list of Users.
	*/
	using UserList = NamedMnodeList<User>;
	using UserListPtr = std::unique_ptr<UserList>;


	/**
	 * UserGroup represents a hierarchy of groups of Users.
	*/
	using UserGroup = Group<User>;
	using UserGroupPtr = std::unique_ptr<UserGroup>;


	/*
	 * A AnyUserGroup is a group of users containing only one "any" user.
	 *
	*/
	class AnyUserGroup : public UserGroup
	{
	public:
		AnyUserGroup();
		~AnyUserGroup();

		/**
		 * Clones this group.
		*/
		virtual UserGroup* clone() const override;

		/**
		 * Returns a bddtrue decision diagram.
		*/
		virtual bdd make_bdd() const override;
	};

}
