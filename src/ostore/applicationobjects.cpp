/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#include "applicationobjects.h"

#include <cassert>
#include <stdexcept>

#include "model/service.h"
#include "model/serviceparser.h"

#include "tools/strutil.h"

namespace fos {

	ApplicationObjects::ApplicationObjects(const csv_reader_config& reader_config) :
		ObjectDictionary<ApplicationObject>{ std::string {"applications" }, reader_config }
	{
	}


	const ApplicationObject* ApplicationObjects::make_object(const csv::CsvValues& values) const
	{
		const ApplicationObject* result = nullptr;

		// values[0] contains the object name
		// values[1] contains a list of protocols
		assert(values.size() == parameters().size());

		if (values[0].empty()) {
			_logger->warning(
				"skip application object, name is empty",
				values[0].c_str()
			);
		}
		else if (values[1].empty()) {
			_logger->warning(
				"skip application object '%s', protoport is not defined",
				values[0].c_str()
			);
		}
		else if (values[0] == "icmp" || values[0] == "udp" || values[0] == "tcp" || values[0] == "any") {
			_logger->warning(
				"application object '%s' can't be overwritten",
				values[0].c_str()
			);
		}
		else {
			try {
				const std::vector<std::string> services = split(values[1]);

				for (const std::string& service : services) {
					// any is a valid service which but currently not recognized
					// by the protocol_port parser.
					if (service == "any")
						continue;

					// validate the protocol/port syntax
					fwm::parse_protocol_port(service);
				}

				result = new ApplicationObject(values[0], services);
			}
			catch (const fwm::ServiceError& e) {
				_logger->warning(
					"application object '%s' skipped, can not extract a protocol and port from '%s'",
					values[0].c_str(),
					e.proto_port().c_str()
				);
			}
		}

		return result;
	}


	const ObjectParameters& ApplicationObjects::parameters()
	{
		static ObjectParameters parameters{
			{ "name", "", false },
			{ "protoport", "any", false }
		};

		return parameters;
	}

}
