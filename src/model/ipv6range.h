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
	* Ipv6Range represents the range of all IPv6 addresses.  This class differs from the parent
	* class by providing a to_string method that outputs a range as a subnet when it is possible.
	*/
	class Ipv6Range final : public Range128
	{
	public:
		/**
		 * Allocates an IPv6Range
		 * @param lbound Lower bound of the range.
		 * @param uboubd Upper bound of the range.
		 * @param ipv4_format
		 */
		explicit Ipv6Range(uint128_t lbound, uint128_t ubound, bool ipv4_format);

		/* returns the address range as a string.
		*/
		virtual std::string to_string() const override;

	private:
		// true when to_string must format the range as an IPv4 address
		const bool _ipv4_format;
	};

}
