/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#pragma once

#include "model/network.h"
#include "model/firewall.h"

#include "ostore/ostoreconfig.h"
#include "ostore/objectstore.h"
#include "ostore/firewallfactory.h"
#include "tools/logger.h"

#include "clictrlchandler.h"
 
namespace cli {
	using namespace fos;
	using namespace fwm;

	/**
	 * A context shared with all command line components.
	 */
	class CliContext {
	public:
		CliContext(const OstoreConfig& config);

		// A store for all firewall objects.
		ObjectStore os;

		// The whole network (owns all firewalls)
		Network nw;

		// The firewall factory
		FirewallFactory ff;

		// The current selected firewall (null if not selected)
		Firewall* fw;

		// The CtrlC interrupt handler
		CliCtrlcHandler ctrlc_handler;

		// A reference to the application logger
		Logger* const logger;
	};

}
