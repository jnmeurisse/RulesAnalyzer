/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#pragma once
#include <exception>
#include <string>
#include <memory>

#include "mnode.h"
#include "group.h"
#include "interval.h"
#include "service.h"
#include "table.h"

#include "tools/strutil.h"

namespace fwm {

	class Application : public NamedMnode
	{
	public:
		static Application* create(const std::string& name, uint32_t app_id, ServiceGroupPtr& services, const ModelOptions& options, bool use_app_svc);
		static Application* any();

		/* Creates a binary decision diagram for this application.
		*/
		virtual bdd make_bdd() const override;

		/* Returns the standard services for this application.
		*/
		inline const ServiceGroup& services() const { return *_services; }

		/* Returns the application id.
		*/
		inline int id() const { return _application.lower(); }

		virtual std::string to_string() const override;

	protected:
		Application(const std::string& name, uint32_t lower, uint32_t upper, ServiceGroupPtr& services, const ModelOptions& options, bool use_app_svc);

	private:
		const Interval _application;
		const bool _use_app_svc;
		const ServiceGroupPtr _services;

		Application(const std::string& name, uint32_t app_id, ServiceGroupPtr& services, const ModelOptions& options, bool use_app_svc);
	};


	class ApplicationGroup final : public Group<Application>
	{
	public:
		ApplicationGroup() = delete;
		ApplicationGroup(const std::string& name);
		ApplicationGroup(const std::string& name, const Application* application);

		/* Allocates a group containing all default services allowed by the
		*  applications of this group.
		*/
		ServiceGroupPtr default_services() const;
	};

	using ApplicationGroupPtr = std::unique_ptr<fwm::ApplicationGroup>;

}
