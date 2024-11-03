/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#include "ostore/serviceobjects.h"

#include <cassert>
#include <string>

#include "model/serviceparser.h"


namespace fos {

	ServiceObjects::ServiceObjects(const CsvReaderConfig& reader_config) :
		ObjectDictionary<ServiceObject>{ std::string{ "services"}, reader_config }
	{
	}


	const ServiceObject* ServiceObjects::make_object(const csv::CsvValues& values) const
	{
		const ServiceObject* result = nullptr;

		// values[0] contains the proto_port object name
		// values[1] contains the list of services.
		assert(values.size() == parameters().size());

		if (values[0].empty()) {
			_logger->warning(
				"skip service object, name is empty"
			);
		}
		else if (values[1].empty()) {
			_logger->warning(
				"skip service object '%s', service is not defined",
				values[0].c_str()
			);
		}
		else if (values[0] == "icmp" || values[0] == "udp" || values[0] == "tcp" || values[0] == "any") {
			_logger->warning(
				"service object '%s' can't be overwritten",
				values[0].c_str()
			);
		}
		else {
			try {
				const std::vector<std::string> proto_ports = split(values[1]);

				for (const std::string& proto_port : proto_ports) {
					// any is a valid protoport
					if (proto_port == "any")
						continue;

					// check the syntax of all other services
					fwm::parse_protocol_port(proto_port);
				}

				result = new ServiceObject(values[0], proto_ports);
			}
			catch (const fwm::ServiceError& e) {
				_logger->warning(
					"service object '%s' skipped, can not extract a protocol or port range from '%s'",
					values[0].c_str(),
					e.proto_port().c_str()
				);
			}

		}

		return result;
	}


	const ObjectParameters& ServiceObjects::parameters()
	{
		static ObjectParameters parameters{
			{ "name",      "", false },
			{ "protoport", "", false }
		};

		return parameters;
	}

}
