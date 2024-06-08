/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#include "application.h"

#include <algorithm>
#include <iterator>
#include <memory>

#include "domain.h"
#include "domains.h"
#include "service.h"

#include "tools/strutil.h"

namespace fwm {

	class AnyApplication : public Application
	{
	public:
		AnyApplication() :
			Application(
				"any",
				0,
				Domains::get(DomainType::Application).upper(),
				std::make_unique<ServiceGroup>("$root", Service::any()),
				ModelOptions::empty(),
				true
			)
		{}


		~AnyApplication()
		{
			delete services().items().front();
		}


		virtual bdd make_bdd() const override
		{
			return bddtrue;
		}
	};


	Application::Application(const std::string& name, uint32_t lower, uint32_t upper, ServiceGroupPtr& services, const ModelOptions& options, bool use_app_svc) :
		NamedMnode(name, options),
		_application{ DomainType::Application, lower, upper },
		_use_app_svc{ use_app_svc },
		_services{ std::move(services) }
	{
	}


	Application::Application(const std::string& name, uint32_t app_id, ServiceGroupPtr& services, const ModelOptions& options, bool use_app_svc) :
		Application(name, app_id, app_id, services, options, use_app_svc)
	{
	}


	Application* Application::create(const std::string& name, uint32_t app_id, ServiceGroupPtr& services, const ModelOptions& options, bool use_app_svc)
	{
		Application* application;

		if (iequal(name, "any")) {
			services.reset();
			application = any();
		}
		else {
			application = new Application(name, app_id, services, options, use_app_svc);
		}

		return application;
	}


	Application* Application::any()
	{
		return new AnyApplication();
	}


	bdd Application::make_bdd() const
	{
		bdd result{ bddtrue };

		if (options().contains(ModelOption::Application)) {
			// Application mode is enabled in the model.
			result &= _application.make_bdd();
		}

		if (_use_app_svc) {
			// Default services enabled on this application.
			result &= _services->make_bdd();
		}

		return result;
	}


	std::string Application::to_string() const
	{
		return _application.to_string();
	}


	ApplicationGroup::ApplicationGroup(const std::string& name) :
		Group<Application>(name)
	{
	}


	ApplicationGroup::ApplicationGroup(const std::string& name, const Application* application) :
		Group<Application>(name, application)
	{
	}


	/* AppDefaultServiceGroup represents a copy of default services allowed on a specific
	*  application.
	*/
	class AppDefaultServiceGroup : public ServiceGroup
	{
	public:
		AppDefaultServiceGroup() :
			ServiceGroup("$app-default-svc")
		{
		}


		/* This group contains the default services of an application.
		*/
		virtual bool is_app_services() const override {
			return true;
		}


		/* Outputs this default service to a table cell.
		*/
		virtual void output(Cell& cell, MnodeInfoType info_type) const override
		{
			if (info_type == MnodeInfoType::NAME)
				cell.append("app-default");
			else
				ServiceGroup::output(cell, info_type);
		}
	};


	ServiceGroupPtr ApplicationGroup::default_services() const
	{
		ServiceGroupPtr services{ std::make_unique<AppDefaultServiceGroup>() };

		for (const Application* application : items()) {
			for (const Service* service : application->services().items()) {
				services->add_member(service);
			}
		}

		return services;
	}

}
