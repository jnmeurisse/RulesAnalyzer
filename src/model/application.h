/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#pragma once
#include "global.h"

#include <cstdint>
#include <string>
#include <memory>

#include "model/mnode.h"
#include "model/mvalue.h"
#include "model/group.h"
#include "model/service.h"
#include "tools/strutil.h"


namespace fwm {

	class Application : public NamedMnode
	{
	public:
		/**
		 * Copy constructor
		*/
		Application(const Application& other);

		/**
		 * Copy constructor using a specific service group.
		*/
		Application(const Application& other, ServiceGroup* services);

		/**
		 * Allocates an Application.
		 *
		 * @param name The name of the application.
		 * @param app_id The application id.
		 * @param services A group of default services for this application.  The group must be
		 *                 dynamically allocated.  The group is owned by this class.
		 * @param options A set of model options.
		 * @param use_app_svc A flag that indicates whether we are using default services when
		 *                    calculating the bdd.
		 *
		 * @return An application.
		 */
		static const Application* create(const std::string& name, uint16_t app_id, ServiceGroup* services,
					const ModelOptions& options, bool use_app_svc);

		/**
		 * Allocates a "any" application.
		 *
		 * @return An Application representing all applications.
		*/
		static const Application* any();

		/**
		 * Creates a binary decision diagram for this application.
		*/
		virtual bdd make_bdd() const override;

		/**
		 * Returns a reference to the standard services for this application.
		*/
		inline const ServiceGroup& services() const noexcept { return *_services; }

		/**
		 * Returns the application value as a string.
		*/
		virtual std::string to_string() const override;

	protected:
		Application(const std::string& name, uint16_t app_id, ServiceGroup* services,
			const ModelOptions& options, bool use_app_svc);
		Application(const std::string& name, Range* range, ServiceGroup* services,
			const ModelOptions& options, bool use_app_svc);

	private:
		const MvaluePtr _app_value;
		const bool _use_app_svc;
		const ServiceGroupPtr _services;
	};


	/**
	 * An ApplicationList represents a list of Applications.
	*/
	using ApplicationList = NamedMnodeList<Application>;
	using ApplicationListPtr = std::unique_ptr<ApplicationList>;


	/**
	 * ApplicationGroup represents a hierarchy of groups of Applications.
	*/
	class ApplicationGroup : public Group<Application>
	{
	public:
		using Group<Application>::Group;

		virtual ApplicationGroup* clone() const override;

		/**
		 * Allocates a group containing all default services allowed by the
		 * applications of this group.
		*/
		ServiceGroupPtr default_services() const;
	};

	using ApplicationGroupPtr = std::unique_ptr<fwm::ApplicationGroup>;


	/**
	 * A AnyApplicationGroup is a group of application containing only an "any" application.
	 *
	*/
	class AnyApplicationGroup final : public ApplicationGroup
	{
	public:
		AnyApplicationGroup();
		virtual ~AnyApplicationGroup();

		/**
		 * Clones this group.
		*/
		virtual ApplicationGroup* clone() const override;

		/**
		 * Returns a bddtrue decision diagram.
		*/
		virtual bdd make_bdd() const override;
	};

}
