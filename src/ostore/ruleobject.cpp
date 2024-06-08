/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#include "ruleobject.h"

namespace fos {

	bool RuleObject::default_app_services() const
	{
		return services.size() == 1 &&
			(iequal(services[0], "application-default") || iequal(services[0], "app-default"));
	}


	void RuleObject::clear()
	{
		row_number = -1;
		name.clear();
		status.clear();
		action.clear();
		source_zones.clear();
		destination_zones.clear();
		source_addresses.clear();
		destination_addresses.clear();
		services.clear();
		applications.clear();
		users.clear();
	}

}
