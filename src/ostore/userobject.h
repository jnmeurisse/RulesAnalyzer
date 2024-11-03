/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#pragma once
#include "global.h"

#include <string>

#include "ostore/firewallobject.h"


namespace fos {

	/*
	*/
	class UserObject : public FirewallObject
	{
	public:
		UserObject(const std::string& name);

		/* Returns a string representation of this object
		*/
		virtual std::string to_string() const override;
	};
}
