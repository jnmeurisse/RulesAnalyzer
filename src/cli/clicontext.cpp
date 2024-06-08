/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#include "clicontext.h"

#include "model/network.h"
#include "ostore/objectstore.h"
#include "ostore/firewallfactory.h"


namespace cli {

	cli::CliContext::CliContext(const OstoreConfig& config) :
		os{ fos::ObjectStore{ config.loader.csv_config, config.fqdn_resolver } },
		nw{ fwm::Network{} },
		ff{ fos::FirewallFactory{ os, config.loader } },
		fw{ nullptr },
		ctrlc_handler{},
		logger{ Logger::get_logger() }
	{
	}

}
