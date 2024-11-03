/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#pragma once
#include "global.h"

#include <string>
#include "cli/clictrlchandler.h"
#include "model/network.h"
#include "model/firewall.h"
#include "model/zone.h"
#include "ostore/ostoreconfig.h"
#include "ostore/objectstore.h"
#include "ostore/firewallfactory.h"
#include "tools/logger.h"


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
		ObjectStore ostore;

		// The whole network (owns all firewalls)
		Network network;

		/**
		 * Returns a pointer to a firewall in the network having the given name.
		 *
		 * @param name The name of the firewall.
		 * @return A pointer to the firewall or a null pointer if the firewall
		 *         does not exist.
		 */
		Firewall* get_firewall(const std::string& name) const;

		/**
		 * Adds a new firewall to the network and make it current.
		 *
		 * @param name Name of the new firewall.
		 * @return A reference to the new firewall
		 *
		 * @throws a runtime error if a firewall having the same name already
		 *         exists in the network.
		*/
		Firewall& add_firewall(const std::string& name);

		/**
		 * Deletes a firewall from the network.
		 *
		 * @param name The firewall name to delete.
		 *
		 * @throws a runtime error if the firewall does not exist in the network
		 *         or if this firewall is current.
		*/
		void delete_firewall(const std::string& name);

		/**
		 * Returns the current firewall.
		 *
		 * @return A reference to the current firewall.
		 *
		 * @throws a runtime error if a firewall is not selected.
		*/
		Firewall& get_current_firewall() const;

		/**
		 * Sets the current firewall.
		 *
		*/
		void set_current_firewall(Firewall* firewall);

		/**
		 * Makes a copy of the current firewall into a new firewall.
		 *
		 * @param name The name of the new firewall.
		 *
		 * @throws a runtime error if a firewall having the same name already
		 *         exists in the network.
		*/
		void clone_current_firewall(const std::string& name);

		LoaderStatus load_current_firewall(const std::string& filename, const CliCtrlcGuard& ctrlc_guard);

		/**
		 * Returns a reference to the "any" source zone object stored in the network cache.
		 *
		 * @return A pointer to the "any" source zone named node.
		 */
		const SrcZone* get_any_src_zone() const;

		/**
		 * Returns a reference to the "any" destination zone object stored in the network cache.
		 *
		 * @return A pointer to the "any" destination zone named node.
		 */
		const DstZone* get_any_dst_zone() const;

		// The CtrlC interrupt handler
		CliCtrlcHandler ctrlc_handler;

		// A reference to the application logger
		Logger* const logger;

	private:
		// The current selected firewall (null if not selected)
		Firewall* _current_firewall;


		// The firewall factory
		FirewallFactory _factory;
	};

}
