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
	class UrlObject : public FirewallObject
	{
	public:
		UrlObject(const std::string& url);

		/* Returns a string representation of this object
		*/
		virtual std::string to_string() const override;
	};
}
