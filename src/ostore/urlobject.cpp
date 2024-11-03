/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#include "ostore/urlobject.h"

namespace fos {

	UrlObject::UrlObject(const std::string& url) :
		FirewallObject(url)
	{
	}


	std::string UrlObject::to_string() const
	{
		return name();
	}

}
