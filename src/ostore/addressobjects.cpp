/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#include "addressobjects.h"
#include <cassert>
#include <stdexcept>
#include <string>
#include <tuple>

#include "model/ipv4parser.h"
#include "tools/strutil.h"

namespace fos {

	AddressObjects::AddressObjects(const csv_reader_config& reader_config, FqdnResolver& fqdn_resolver) :
		ObjectDictionary<AddressObject>{ std::string{"addresses"}, reader_config },
		_fqdn_resolver{ fqdn_resolver }
	{
	}


	const AddressObject* AddressObjects::get(const std::string& name) const noexcept
	{
		const AddressObject* address{ ObjectDictionary<AddressObject>::get(name) };

		if (address && address->type() == AddressType::fqdn) {
			address = _fqdn_resolver.resolve(address);
		}

		return address;
	}


	static bool is_ip_mask(const std::string& str) {
		return iequal(str, "ipmask") || iequal(str, "IP netmask");
	}


	static bool is_ip_range(const std::string& str) {
		return iequal(str, "iprange") || iequal(str, "IP Range");
	}


	static bool is_fqdn(const std::string& str) {
		return iequal(str, "fqdn");
	}


	static bool is_netmask(const fwm::Ipv4AddressType address_type) {
		return address_type == fwm::Ipv4AddressType::Address || address_type == fwm::Ipv4AddressType::Subnet;
	}


	static bool is_range(const fwm::Ipv4AddressType address_type) {
		return address_type == fwm::Ipv4AddressType::Range;
	}


	const AddressObject* AddressObjects::make_object(const csv::CsvValues& values) const
	{
		const AddressObject* result = nullptr;

		// values[0] contains the address object name.
		// values[1] contains the address object type.
		// values[2] contains an IP address, an IP address range, ... depending on
		// the address type.
		assert(values.size() == parameters().size());

		if (values[0].empty()) {
			_logger->warning(
				"skip address object, name is empty"
			);
		}
		else if (values[2].empty()) {
			_logger->warning(
				"skip address object '%s', address is empty",
				values[0].c_str()
			);
		}
		else {
			if (is_ip_mask(values[1])) {
				const std::vector<std::string> ipmask_addresses = split(values[2]);

				try {
					bool valid = true;

					for (const std::string& addr : ipmask_addresses) {
						auto parse_result = fwm::parse_network_address(addr, false);

						if (!is_netmask(std::get<0>(parse_result))) {
							_logger->warning(
								"address object '%s' skipped, address '%s' is not an IP mask",
								values[0].c_str(),
								addr.c_str()
							);

							valid = false;
							break;
						}
					}

					if (valid)
						result = new IpmaskAddressObject(values[0], ipmask_addresses);
				}
				catch (const fwm::Ipv4AddressError& e) {
					_logger->warning(
						"address object '%s' skipped, can not extract an IP address from '%s'",
						values[0].c_str(),
						e.address().c_str()
					);
				}
			}
			else if (is_ip_range(values[1])) {
				const std::vector<std::string> iprange_addresses = split(values[2]);

				try {
					bool valid = true;

					for (const std::string& addr : iprange_addresses) {
						auto parse_result = fwm::parse_network_address(addr, false);

						if (!is_range(std::get<0>(parse_result))) {
							_logger->warning(
								"address object '%s' skipped, address '%s' is not an IP range",
								values[0].c_str(),
								addr.c_str()
							);

							valid = false;
							break;
						}
					}

					if (valid)
						result = new IprangeAddressObject(values[0], iprange_addresses);
				}
				catch (const fwm::Ipv4AddressError& e) {
					_logger->warning(
						"address object '%s' skipped, can not extract an IP range from '%s'",
						values[0].c_str(),
						e.address().c_str()
					);
				}
			}
			else if (is_fqdn(values[1])) {
				result = new FqdnAddressObject(values[0], values[2]);
			}
			else if (iequal(values[1], "")) {
				const std::vector<std::string> addresses = split(values[2]);

				if (addresses.size() == 1 && !fwm::is_network_address(addresses[0], false)) {
					result = new FqdnAddressObject(values[0], addresses[0]);
				}
				else {
					try {
						bool valid = true;

						// detect the type of the first address
						auto parse_result = fwm::parse_network_address(addresses[0], false);
						bool netmask_address_type = is_netmask(std::get<0>(parse_result));

						for (const std::string& addr : addresses) {
							auto parse_result = fwm::parse_network_address(addr, false);

							if ((netmask_address_type && !is_netmask(std::get<0>(parse_result))) ||
								(!netmask_address_type && is_netmask(std::get<0>(parse_result)))) {
								_logger->warning(
									"address object '%s' skipped, address '%s' is not %s",
									values[0].c_str(),
									addr.c_str(),
									netmask_address_type ? "ipmask" : "iprange"
								);

								valid = false;
								break;
							}
						}

						if (valid) {
							if (netmask_address_type)
								result = new IpmaskAddressObject(values[0], addresses);
							else
								result = new IprangeAddressObject(values[0], addresses);
						}
					}
					catch (const fwm::Ipv4AddressError& e) {
						_logger->warning(
							"address object '%s' skipped, can not extract an IP address from '%s'",
							values[0].c_str(),
							e.address().c_str()
						);
					}
				}
			}
			else {
				_logger->warning(
					"address object '%s' skipped, '%s' is an invalid address type",
					values[0].c_str(),
					values[1].c_str()
				);
			}
		}

		return result;
	}


	const ObjectParameters& AddressObjects::parameters()
	{
		static ObjectParameters parameters{
			{ "name", "", false },
			{ "type", "", true },
			{ "address", "", false }
		};

		return parameters;
	}

}
