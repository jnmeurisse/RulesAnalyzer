
/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#include "ipconvertor.h"

#if defined(RA_OS_WINDOWS)
 #include <WS2tcpip.h>
#else
 #include <arpa/inet.h>
#endif

#include "tools/strutil.h"


namespace fwm {

	bool decode_ipv4_address(const std::string& address_text, uint32_t & address)
	{
		struct in_addr inet4_addr;
		bool status = false;

		if (::inet_pton(AF_INET, address_text.c_str(), &inet4_addr) == 1) {
			address = ntohl(inet4_addr.s_addr);
			status = true;
		}

		return status;
	}


	bool encode_ipv4_address(uint32_t address, std::string& address_text)
	{
		char buffer[16] = { 0 };
		bool status = false;

		struct in_addr inet4_addr;
		inet4_addr.s_addr = htonl(address);
		if (::inet_ntop(AF_INET, &inet4_addr, buffer, sizeof(buffer))) {
			address_text = buffer;
			status = true;
		}

		return status;
	}


	bool decode_ipv6_address(const std::string& address_text, uint128_t& address)
	{
		struct in6_addr inet6_addr;
		bool status = false;

		if (::inet_pton(AF_INET6, address_text.c_str(), &inet6_addr) == 1) {
			// Convert the ipv6 address to host order and store it into a 128 bits
			address = uint128_t(0);
			for (int i = 0; i < 16; i += 4) {
				uint32_t *segment = (uint32_t *)&inet6_addr.s6_addr[i];
				address = (address << 32) | ntohl(*segment);
			}

			status = true;
		}

		return status;
	}


	bool encode_ipv6_address(uint128_t address, std::string& address_text)
	{
		char buffer[40] = { 0 };
		bool status = false;

		// Convert the ipv6 address to network order
		struct in6_addr inet6_addr;
		for (int i = 12; i >= 0; i -= 4) {
			uint32_t *segment = (uint32_t *)&inet6_addr.s6_addr[i];
			*segment = htonl(uint32_t(address));
			address >>= 32;
		}

		if (::inet_ntop(AF_INET6, &inet6_addr, buffer, sizeof(buffer))) {
			address_text = buffer;
			status = true;
		}

		return status;
	}

}
