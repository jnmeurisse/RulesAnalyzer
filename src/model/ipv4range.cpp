/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#include "model/ipv4range.h"

#include <stdexcept>

#include "model/ipconvertor.h"
#include "tools/strutil.h"
#include "fmt/core.h"


namespace fwm {
	static size_t bit_count(uint32_t value)
	{
		size_t count{ 0 };

		while (value) {
			if (value & 0x01)
				count++;		// bit is set
			value >>= 1;
		}

		return count;
	}


	Ipv4Range::Ipv4Range(uint32_t lbound, uint32_t ubound) :
		Range32(32, lbound, ubound)
	{
	}


	std::string Ipv4Range::to_string() const
	{
		uint32_t diff = _ubound - _lbound;

		std::string low_address;
		if (!encode_ipv4_address(_lbound, low_address))
			throw std::runtime_error("error in encode_ipv4_address");

		// Format the address range
		std::string result;
		if (is_singleton()) {
			result = low_address;
		}
		else if (is_power_of_2()) {
			result = fmt::format("{}/{}", low_address, 32 - bit_count(diff));
		}
		else if (diff < 1024) {
			result = fmt::format("{}+{}", low_address, diff);
		}
		else {
			std::string high_address;
			if (!encode_ipv4_address(_ubound, high_address))
				throw std::runtime_error("error in encode_ipv4_address");
			result = fmt::format("{}-{}", low_address, high_address);
		}

		return result;
	}

}
