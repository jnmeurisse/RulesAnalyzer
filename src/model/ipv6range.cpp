/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#include "model/ipv6range.h"

#include <cstdint>
#include <memory>
#include <stdexcept>

#include "model/domain.h"
#include "model/ipconvertor.h"
#include "model/ipv4range.h"
#include "fmt/core.h"


namespace fwm {
	static size_t bit_count(uint128_t value)
	{
		size_t count{ 0 };

		while (value) {
			if (value & 0x01)
				count++;		// bit is set
			value >>= 1;
		}

		return count;
	}


	Ipv6Range::Ipv6Range(uint128_t lbound, uint128_t ubound, bool ipv4_format) :
		Range128(128, lbound, ubound),
		_ipv4_format{ ipv4_format }
	{
		if (ipv4_format && (lbound > uint128_t(Address4Domain::max()) ||
		                    ubound > uint128_t(Address4Domain::max())))
			throw std::runtime_error("internal error : can not convert an IPv6 to IPv4 format");
	}


	std::string Ipv6Range::to_string() const
	{
		std::string result;

		if (_ipv4_format) {
			RangePtr ipv4_range = std::make_unique<Ipv4Range>(
									static_cast<uint32_t>(_lbound),
									static_cast<uint32_t>(_ubound)
							);
			result = ipv4_range->to_string();
		}
		else {
			uint128_t diff = _ubound - _lbound;

			std::string low_address;
			if (!encode_ipv6_address(_lbound, low_address))
				throw std::runtime_error("error in encode_ipv6_address");

			// Format the address range
			if (is_singleton()) {
				result = low_address;
			}
			else if (is_power_of_2()) {
				result = fmt::format("{}/{}", low_address, 128 - bit_count(diff));
			}
			else if (diff < uint128_t(1024)) {
				result = fmt::format("{}+{}", low_address, (uint64_t) diff);
			}
			else {
				std::string high_address;
				if (!encode_ipv6_address(_ubound, high_address))
					throw std::runtime_error("error in encode_ipv6_address");
				result = fmt::format("{}-{}", low_address, high_address);
			}
		}

		return result;
	}

}
