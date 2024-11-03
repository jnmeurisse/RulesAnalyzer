#pragma once
#include "global.h"

#include <fstream>
#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include "ostore/addressobject.h"
#include "ostore/ostoreconfig.h"
#include "tools/logger.h"


namespace fos {

	class FqdnResolver {
	public:
		FqdnResolver(const FqdnResolverConfig& resolver_config);

		/* Initialize the Fqdn resolver.
		*  The method initializes the winsock interface and load resolved fqdn
		*  from the file cache.
		*/
		void initialize();

		/* Terminate the Fqdn resolver.
		*  Cleanup the winsock interface and close th fqdn file cache.
		*/
		void terminate();

		/* Resolves the given fqdn address stored in the given address object.
		*  The method returns an address object containing all IP addresses
		*  resolved by a DNS query or by looking into the fqdn file cache.
		*/
		const AddressObject* resolve(const AddressObject* fqdn, IPAddressModel ip_model, bool strict);

	private:
		// The application logger
		Logger* _logger;

		// A reference to the resolver configuration.
		const FqdnResolverConfig& _resolver_config;

		// True when windows socket is initialized.
		bool _ws_initialized;

		// Specify the filename storing resolved FQDNs.
		// The file contains one line per FQDN.
		// The line contains the following values separated by a comma :
		//    fqdn
		//    n
		//    ip1;ip2;ip3...;ipn
		//
		// For example
		//    dns.google.com,2,8.8.4.4;8.8.8.8;2001:4860:4860::8888;2001:4860:4860::8844
		std::ofstream _file_cache;

		// All FQDN resolved to IPv4 and IPv6 addresses in memory.
		// The pointer is null when the FQDN has not resolution.
		std::map<const std::string, std::unique_ptr<const AddressObject>> _memory_cache;

		// FQDN resolved to IPv4 or IPv6 addresses in memory depending on the current IP address model.
		// The pointer is null when the FQDN has not resolution.
		std::map<const std::string, std::unique_ptr<const AddressObject>> _filtered_memory_cache;

		// Describe the result of the resolve_hostname method.
		// The first value is the return code of the getaddrinfo function.
		// The second value is a vector of IP addresses.  This vector is
		// empty when the first value == EAI_NONAME.
		using resolve_result = std::pair<int, std::vector<std::string>>;

		// A method that resolves a hostname to IP addresses.
		resolve_result resolve_hostname(const std::string& hostname);

		// Loads resolved hostname from file cache.
		void load_cache(const std::string& filename);
	};

}
