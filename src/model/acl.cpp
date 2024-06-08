/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#include "acl.h"

#include "tools/strutil.h"

namespace fwm {
	Acl::Acl(const RuleList& rule_list) :
		RuleList(rule_list.filter(RuleStatus::ENABLED))
	{
	}
}
