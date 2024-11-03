/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#include "ostore/addressobjects.h"

#include <cassert>
#include <stdexcept>
#include <string>
#include <tuple>

#include "model/ipaddress.h"
#include "model/address.h"
#include "tools/strutil.h"

namespace fos {

	AddressObjects::AddressObjects(const CsvReaderConfig& reader_config, const ModelConfig& core_config, FqdnResolver& fqdn_resolver) :
		ObjectDictionary<AddressObject>{ std::string{"addresses"}, reader_config },
		_ip_model{ core_config.ip_model },
		_strict_ip_parser{ core_config.strict_ip_parser },
		_fqdn_resolver{ fqdn_resolver }
	{
	}


	const AddressObject* AddressObjects::get(const std::string& name) const
	{
		const AddressObject* address{ ObjectDictionary<AddressObject>::get(name) };

		if (address && address->type() == AddressType::fqdn) {
			address = _fqdn_resolver.resolve(address, _ip_model, _strict_ip_parser);
		}

		return address;
	}


	static bool is_ip_mask(const std::string& str) {
		return rat::iequal(str, "ipmask") || rat::iequal(str, "IP netmask");
	}


	static bool is_ip_range(const std::string& str) {
		return rat::iequal(str, "iprange") || rat::iequal(str, "IP range");
	}


	static bool is_fqdn(const std::string& str) {
		return rat::iequal(str, "fqdn");
	}


	static bool is_netmask(const fwm::IPAddressType address_type) {
		return address_type == IPAddressType::Address || address_type == IPAddressType::Subnet;
	}


	static bool is_range(const fwm::IPAddressType address_type) {
		return address_type == fwm::IPAddressType::Range;
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
						if (!is_netmask(get_ip_address_type(addr))) {
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
				catch (const fwm::IPAddressError& e) {
					_logger->warning(
						"address object '%s' skipped, '%s' is not a valid IP address (%s)",
						values[0].c_str(),
						e.address().c_str(),
						e.reason().c_str()
					);
				}
			}
			else if (is_ip_range(values[1])) {
				const std::vector<std::string> iprange_addresses = split(values[2]);

				try {
					bool valid = true;

					for (const std::string& addr : iprange_addresses) {
						if (!is_range(get_ip_address_type(addr))) {
							_logger->warning(
								"address object '%s' skipped, '%s' is not a valid IP range",
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
				catch (const fwm::IPAddressError& e) {
					_logger->warning(
						"address object '%s' skipped, '%s' is not a valid IP range (%s)",
						values[0].c_str(),
						e.address().c_str(),
						e.reason().c_str()
					);
				}
			}
			else if (is_fqdn(values[1])) {
				result = new FqdnAddressObject(values[0], values[2]);
			}
			else if (rat::iequal(values[1], "file")) {
				std::ifstream stream{ values[2], std::ifstream::in };
				if (!stream.good())
					_logger->warning(
						"address object '%s' skipped, can't open file '%s'.",
						values[0].c_str(),
						values[1].c_str()
					);
				else {
					std::vector<std::string> external_address_list;

					int skip_counter = 0;
					while (true) {
						std::string address;
						if (!std::getline(stream, address))
							break;

						// skip empty line
						address = rat::trim(address);
						if (address.size() == 0)
							continue;

						// remove cr if the file was created on Windows.
						if (address.size() > 0 && address[address.size()-1] == '\r' )
							address = address.substr( 0, address.size() - 1 );

						if (is_ip_address(address) && is_netmask(get_ip_address_type(address)))
							external_address_list.push_back(address);
						else
							skip_counter++;
					}

					if (external_address_list.size() > 0) {
						if (skip_counter > 0) {
							_logger->warning(
								"address object '%s' partially loaded, %d addresses skipped in file '%s'.",
								values[0].c_str(),
								skip_counter,
								values[1].c_str()
							);
						}

						result = new IpmaskAddressObject(values[0], external_address_list);
					}
					else
						_logger->warning(
							"address object '%s' skipped, file '%s' is empty.",
							values[0].c_str(),
							values[1].c_str()
						);
				}
			}
			else if (rat::iequal(values[1], "")) {
				const std::vector<std::string> addresses = split(values[2]);

				if (addresses.size() == 1 && !is_ip_address(addresses[0])) {
					result = new FqdnAddressObject(values[0], addresses[0]);
				}
				else {
					try {
						bool valid = true;

						// detect the type of the first address
						bool is_address0_netmask_type = is_netmask(get_ip_address_type(addresses[0]));

						for (const std::string& addr : addresses) {
							bool is_address_netmask_type = is_netmask(get_ip_address_type(addr));

							if ((is_address0_netmask_type && !is_address_netmask_type) ||
								(!is_address0_netmask_type && is_address_netmask_type)) {
								_logger->warning(
									"address object '%s' skipped, address '%s' is not %s",
									values[0].c_str(),
									addr.c_str(),
									is_address0_netmask_type ? "ipmask" : "iprange"
								);

								valid = false;
								break;
							}
						}

						if (valid) {
							if (is_address0_netmask_type)
								result = new IpmaskAddressObject(values[0], addresses);
							else
								result = new IprangeAddressObject(values[0], addresses);
						}
					}
					catch (const fwm::IPAddressError& e) {
						_logger->warning(
							"address object '%s' skipped, '%s' is not a valid IP address (%s)",
							values[0].c_str(),
							e.address().c_str(),
							e.reason().c_str()
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


	bool AddressObjects::is_ip_address(const std::string& addr) const
	{
		return fwm::is_ip_address(addr, _ip_model, _strict_ip_parser);
	}


	IPAddressType AddressObjects::get_ip_address_type(const std::string& addr) const
	{
		return fwm::get_ip_address_type(addr, _ip_model, _strict_ip_parser);
	}


	const ObjectParameters& AddressObjects::parameters()
	{
		static ObjectParameters parameters{
			{ "name",    "", false },
			{ "type",    "", true },
			{ "address", "", false }
		};

		return parameters;
	}

}
