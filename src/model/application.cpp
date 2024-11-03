/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#include "model/application.h"

#include <algorithm>
#include <iterator>
#include <memory>

#include "model/domain.h"
#include "model/domains.h"
#include "model/service.h"
#include "tools/strutil.h"


namespace fwm {

	class AnyApplication final : public Application
	{
	public:
		AnyApplication() :
			Application(
				"any",
				ApplicationDomain::create_full_range(),
				new AnyServiceGroup(),
				ModelOptions::empty(),
				true
			)
		{
		}

		virtual bdd make_bdd() const override
		{
			return bddtrue;
		}

		virtual std::string to_string() const override
		{
			return "any";
		}
	};


	Application::Application(const std::string& name, Range* range, ServiceGroup* services,
				const ModelOptions& options, bool use_app_svc) :
		NamedMnode(name, options),
		_app_value{ std::make_unique<Mvalue>(DomainType::Application, range) },
		_use_app_svc{ use_app_svc },
		_services{ services }
	{
	}


	Application::Application(const std::string& name, uint16_t app_id, ServiceGroup* services,
				const ModelOptions& options, bool use_app_svc) :
		Application(name, ApplicationDomain::create_singleton(app_id), services, options, use_app_svc)
	{
	}


	Application::Application(const Application& other) :
		Application(other.name(), other._app_value->range().clone(), other._services->clone(), other.options(), other._use_app_svc)
	{
	}


	Application::Application(const Application& other, ServiceGroup* services) :
		Application(other.name(), other._app_value->range().clone(), services, other.options(), true)
	{
	}


	const Application* Application::create(const std::string& name, uint16_t app_id, ServiceGroup* services,
		const ModelOptions& options, bool use_app_svc)
	{
		return new Application(name, app_id, services, options, use_app_svc);
	}


	const Application* Application::any()
	{
		return new AnyApplication();
	}


	bdd Application::make_bdd() const
	{
		bdd result{ bddtrue };

		if (options().contains(ModelOption::Application)) {
			// Application mode is enabled in the model.
			result &= _app_value->make_bdd();
		}

		if (_use_app_svc) {
			// Default services enabled on this application.
			result &= _services->make_bdd();
		}

		return result;
	}


	std::string Application::to_string() const
	{
		return _app_value->to_string();
	}


	ApplicationGroup* ApplicationGroup::clone() const
	{
		return new ApplicationGroup(*this);
	}


	/**
	 * AppDefaultServiceGroup represents a copy of default services allowed on a specific
	 *  application.
	*/
	class AppDefaultServiceGroup : public ServiceGroup
	{
	public:
		AppDefaultServiceGroup() :
			ServiceGroup("$app-default-svc")
		{
		}


		/**
		 * This group contains the default services of an application.
		*/
		virtual bool is_app_services() const override {
			return true;
		}


		/**
		 * Outputs this default service to a table cell.
		*/
		virtual void write_to_cell(Cell& cell, MnodeInfoType info_type) const override
		{
			if (info_type == MnodeInfoType::NAME)
				cell.append("app-default");
			else
				ServiceGroup::write_to_cell(cell, info_type);
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


	AnyApplicationGroup::AnyApplicationGroup() :
		ApplicationGroup("$any-app-group", Application::any())
	{
	}


	AnyApplicationGroup::~AnyApplicationGroup()
	{
		// delete the "any" application allocated in the constructor
		parse([](const Application* application){ delete application; });
	}


	ApplicationGroup* AnyApplicationGroup::clone() const
	{
		return new AnyApplicationGroup();
	}


	bdd AnyApplicationGroup::make_bdd() const
	{
		return bddtrue;
	}

}
