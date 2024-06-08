/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#pragma once

#include <functional>
#include <list>
#include <memory>
#include <vector>

#include "address.h"
#include "application.h"
#include "rule.h"
#include "rulelist.h"
#include "service.h"
#include "user.h"
#include "zone.h"

namespace fwm {
	/**
	 * An access control list (ACL) is an ordered collection of enabled rules.
	 *
	*/
	class Acl : public RuleList
	{
	public:
		Acl(const RuleList& rule_list);
	};

	using AclPtr = std::unique_ptr<const Acl>;
}
