/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#include "serviceobject.h"

#include "tools/strutil.h"

namespace fos {

	ServiceObject::ServiceObject(const std::string& name, const std::vector<std::string>& services) :
		FirewallObject(name),
		_services{ services }
	{
		if (services.empty())
			throw_no_service_error(name);

		for (const std::string service : services) {
			if (service.empty())
				throw std::runtime_error(string_format(
					"service object '%s' has at least one empty service",
					name.c_str())
				);
		}
	}

	ServiceObject::ServiceObject(const std::string& name, const std::string& service) :
		FirewallObject(name),
		_services{ service }
	{
		if (service.empty())
			throw_no_service_error(name);
	}


	std::string ServiceObject::to_string() const
	{
		return strings_join(_services, ", ", false);
	}


	void ServiceObject::throw_no_service_error(const std::string& name)
	{
		throw std::runtime_error(
			string_format(
				"service object '%s' has no service",
				name.c_str()
			)
		);
	}
}
