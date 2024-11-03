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
#include <vector>
#include "firewallobject.h"


namespace fos {

	class ServiceObject : public FirewallObject
	{
	public:
		ServiceObject(const std::string& name, const std::vector<std::string>& services);
		ServiceObject(const std::string& name, const std::string& service);

		/* Returns a string representation of this object
		*/
		virtual std::string to_string() const override;

		/* Returns the service definitions.
		*/
		inline const std::vector<std::string>& services() const noexcept { return _services; }

		/* Returns true if it is a multi services.
		*/
		inline bool is_multi() const noexcept { return _services.size() > 1; }

	private:
		const std::vector<std::string> _services;

		static void throw_no_service_error(const std::string& name);
	};

}