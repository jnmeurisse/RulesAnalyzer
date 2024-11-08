#include "ostore/fqdnresolver.h"

#if defined(RA_OS_WINDOWS)
  #define WIN32_LEAN_AND_MEAN
  #include <Windows.h>
  #include <winsock2.h>
  #include <ws2tcpip.h>
#else
  #include <arpa/inet.h>
  #include <sys/types.h>
  #include <sys/socket.h>
  #include <netdb.h>
#endif
#include <cstring>
#include <iostream>
#include <memory>
#include <stdexcept>

#include "model/address.h"
#include "ostore/addressobject.h"
#include "tools/csvparser.h"
#include "tools/strutil.h"
#include "tools/logger.h"


namespace fos {

	FqdnResolver::FqdnResolver(const FqdnResolverConfig& resolver_config) :
		_logger{ Logger::get_logger() },
		_resolver_config{ resolver_config },
		_ws_initialized{ false }
	{
	}


	void FqdnResolver::initialize()
	{
		if (!_ws_initialized && _resolver_config.enable) {
#if defined(RA_OS_WINDOWS)
			WSAData wsa_data;

			if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0) {
				_logger->error("Failed to initialize Winsock, fqdn are not resolved");
				return;
			}
#endif
			_ws_initialized = true;
		}

		if (_resolver_config.cache) {
			// Load resolved fqdn from cache file
			load_cache(_resolver_config.filename);

			// Append new fqdn at the end of the cache file
			_file_cache.open(_resolver_config.filename, std::ofstream::out | std::ofstream::app);
		}
	}


	void FqdnResolver::terminate()
	{
		if (_ws_initialized) {
#if defined(RA_OS_WINDOWS)
			WSACleanup();
#endif
			_ws_initialized = false;
		}

		if (_resolver_config.cache) {
			if (_file_cache.is_open()) {
				_file_cache.flush();
				_file_cache.close();
			}
		}
	}



	const AddressObject* FqdnResolver::resolve(const AddressObject* fqdn, IPAddressModel ip_model, bool strict)
	{
		const AddressObject* address_object = nullptr;

		if (_resolver_config.enable && fqdn && fqdn->type() == AddressType::fqdn  && fqdn->addresses().size() == 1) {
			// Is the hostname resolved and available in the memory cache ?
			const std::string& hostname = rat::trim(fqdn->addresses()[0]);
			auto it = _filtered_memory_cache.find(hostname);

			if (it != _filtered_memory_cache.end()) {
				// fqdn was resolved and filtered
				address_object = it->second.get();
			}
			else {
				it = _memory_cache.find(hostname);

				if (it != _memory_cache.end()) {
					// fqdn was resolved but not yet filtered
					std::vector<std::string> filtered_addresses;

					// Get the resolved address.  This pointer can be null if the resolution failed.
					const AddressObject* resolved_address_object = it->second.get();

					if (resolved_address_object) {
						// Filter the address according to the IP memory model.
						for (const std::string& address : resolved_address_object->addresses()) {
							if (is_ip_address(address, ip_model, strict))
								filtered_addresses.push_back(address);
						}
					}

					if (filtered_addresses.size() > 0) {
						// When resolved, we create an address object containing the IP addresses
						address_object = new IpmaskAddressObject(fqdn->name(), filtered_addresses);
					}

					// Store the hostname in the memory cache even if not resolved
					_filtered_memory_cache.insert(std::make_pair(hostname, address_object));

				}
				else {
					// fqdn was not resolved before, try to resolve it
					const auto result = resolve_hostname(hostname);

					// Save the addresses into the cache
					const int status = std::get<0>(result);
					const std::vector<std::string>& addresses = std::get<1>(result);

					if (status == 0 || status == EAI_NONAME) {
						if (addresses.size() > 0) {
							// When resolved, we create an address object containing the IP addresses
							address_object = new IpmaskAddressObject(fqdn->name(), addresses);
						}

						// Store the hostname in the memory cache even if not resolved.
						_memory_cache.insert(std::make_pair(hostname, address_object));

						if (_file_cache.is_open() && address_object) {
							// Store the hostname in the file cache only if fqdn if resolved.
							_file_cache
								<< hostname
								<< ","
								<< address_object->addresses().size()
								<< ","
								<< rat::strings_join(address_object->addresses(), ";", false)
								<< std::endl
								<< std::flush;
						}

						// filter (recursive call)
						address_object = resolve(fqdn, ip_model, strict);
					}
				}
			}
		}

		return address_object;
	}


	FqdnResolver::resolve_result FqdnResolver::resolve_hostname(const std::string& hostname)
	{
		if (!_ws_initialized)
			throw std::runtime_error("internal error : winsock not initialized in FqdnReolver");

		// Set up the hints structure to specify the desired options (IPv4 or IPv6, TCP).
		struct addrinfo hints;
		memset(&hints, 0, sizeof(hints));
		hints.ai_family = AF_UNSPEC;
		hints.ai_socktype = SOCK_STREAM; // TCP socket

		// Resolve the hostname.
		_logger->info("resolving fqdn '%s'", hostname.c_str());

		std::vector<std::string> addresses;
		struct addrinfo* addr_info;

		const int status = ::getaddrinfo(hostname.c_str(), NULL, &hints, &addr_info);
		if (status == 0) {
			// Iterate through the results to find IPv4 or IPv6 addresses.
			for (struct addrinfo* rp = addr_info; rp != NULL; rp = rp->ai_next) {
				if (rp->ai_family == AF_INET) {
					struct sockaddr_in* sockaddr_ipv4;
					sockaddr_ipv4 = (struct sockaddr_in *) rp->ai_addr;

					// convert the IPv4 Internet network address into a string in Internet
					// standard format and add it to the address vector.
					char buffer[16] = { 0 };
					if (::inet_ntop(rp->ai_family, &sockaddr_ipv4->sin_addr, buffer, sizeof(buffer)))
						addresses.push_back(buffer);
				}
				else if (rp->ai_family == AF_INET6) {
					struct sockaddr_in6* sockaddr_ipv6;
					sockaddr_ipv6 = (struct sockaddr_in6*) rp->ai_addr;

					// convert the IPv6 Internet network address into a string in Internet
					// standard format and add it to the address vector.
					char buffer[40] = { 0 };
					if (::inet_ntop(rp->ai_family, &sockaddr_ipv6->sin6_addr, buffer, sizeof(buffer)))
						addresses.push_back(buffer);
				}
			}

			// Free all memory allocated by getaddrinfo
			freeaddrinfo(addr_info);
		}
		else if (status != EAI_NONAME) {
			_logger->error(
				"FqdnResolver : getaddrinfo('%s') failed with error %d",
				hostname.c_str(),
				status);
		}

		return std::make_pair(status, addresses);
	}


	void FqdnResolver::load_cache(const std::string& filename)
	{
		std::ifstream csv_stream(filename, std::ios::in);

		if (csv_stream.is_open()) {
			_logger->info("loading fqdn from file '%s'", filename.c_str());

			const csv::CsvParserOptions csv_options{ '"', '\\', ',', false, true };
			csv::CsvParser parser(csv_stream, csv_options);
			csv::CsvValues values;

			while (parser.next_record(values)) {
				if (values.size() == 3) {
					const std::string& fqdn_name = values[0];

					int len;
					std::vector<std::string> addresses;
					if (fqdn_name.size() > 0
						&& rat::str2i(values[1], len) && len >= 0
						&& len == rat::split(values[2], ';', addresses)) {

						AddressObject* address_object = nullptr;
						if (addresses.size() >= 1) {
							address_object = new IpmaskAddressObject{ fqdn_name, addresses };
						}

						_memory_cache.insert(std::make_pair(fqdn_name, address_object));
					}
				}
			}

			csv_stream.close();
			_logger->info("%zu unique %s loaded from file '%s'",
				_memory_cache.size(),
				rat::pluralize(_memory_cache.size(), "hostname").c_str(),
				filename.c_str()
			);
		}
		else {
			_logger->error("Unable to load fqdn from file '%s'", filename.c_str());
		}

	}

}
