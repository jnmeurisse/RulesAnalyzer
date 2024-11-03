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


namespace fos {

	/**
	 * Abstract base class of all firewall configuration objects.
	 * A configuration object is an element that is used to construct a security policy.
	*/
	class FirewallObject abstract
	{
	public:
		/* Returns a string representation of this object
		*/
		virtual std::string to_string() const = 0;

		/* Returns the name of this object
		*/
		inline const std::string& name() const noexcept { return _name; }

	protected:
		FirewallObject(const std::string& name);
		FirewallObject(const FirewallObject& other);

	private:
		const std::string _name;
	};

}

