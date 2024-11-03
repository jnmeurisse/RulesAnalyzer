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
#include "rangeimpl.h"

namespace fwm {

	/**
	* Ipv4Range represents the range of all IPv4 addresses.  This class differs from the parent
	* class by providing a to_string method that outputs a range as a subnet when it is possible.
	*/
	class Ipv4Range final : public Range32
	{
	public:
		/**
		 * Allocates an IPv4Range
		 * @param lbound Lower bound of the range.
		 * @param uboubd Upper bound of the range.
		 */
		explicit Ipv4Range(uint32_t lbound, uint32_t ubound);

		/* returns the address range as a string.
		*/
		virtual std::string to_string() const override;
	};

}
