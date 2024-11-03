/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#include "cliosquery.h"

#include <iostream>
#include <string>
#include "model/table.h"
#include "model/tablewriter.h"
#include "tools/strutil.h"
#include "tools/yesno.h"


namespace cli {

	CliOsQueryCommand::CliOsQueryCommand(CliContext& context) :
		CliCommandMap(context, 1, 2, nullptr)
	{
		add(CommandKeys{ "addr",  "address" },           new CliQueryAdrCommand(context));
		add(CommandKeys{ "addrg", "address-group" },     new CliQueryAdrgCommand(context));
		add(CommandKeys{ "svc",   "service" },           new CliQuerySvcCommand(context));
		add(CommandKeys{ "svcg",  "service-group" },     new CliQuerySvcgCommand(context));
		add(CommandKeys{ "app",   "application" },       new CliQueryAppCommand(context));
		add(CommandKeys{ "appg",  "application-group" }, new CliQueryAppgCommand(context));
		add(CommandKeys{ "usr",   "user" },              new CliQueryUsrCommand(context));
		add(CommandKeys{ "usrg",  "user-group" },        new CliQueryUsrgCommand(context));
		add(CommandKeys{ "url" },                        new CliQueryUrlCommand(context));
		add(CommandKeys{ "urlg",  "url-group" },         new CliQueryUrlgCommand(context));
	}


	CliOsQuerySubCommand::CliOsQuerySubCommand(CliContext& context) :
		CliCommand(context, 1, 1, new CliCommandFlags({CliCommandFlag::OutputToFile}))
	{
	}


	void CliOsQuerySubCommand::do_execute(CliArgs& args, const CliCtrlcGuard& ctrlc_guard)
	{
		const fwm::Table query_result = do_query(args.pop());

		if (args.has_option(CliCommandFlag::OutputToFile)) {
			const std::string& output_file = args.output_file();
			if (write_table(output_file, query_result, ctrlc_guard))
				context.logger->info("results available in '%s'", output_file.c_str());
		}
		else {
			bool confirm = true;
			if (query_result.row_count() > 20) {
				confirm = rat::question_yesno(
					fmt::format("Display all {} results", query_result.row_count()),
					false
				);
			}

			if (confirm) {
				TableTxtWriter writer{ query_result };
				writer
					.margin(4)
					.write(std::cout, ctrlc_guard.get_interrupt_cb());
			}
		}
	}


	fwm::Table CliQueryAdrCommand::do_query(const std::string & query)
	{
		fwm::Table addresses({ "name", "value" }, { 0, 40 });

		for (const fos::AddressObject* address : context.ostore.query_addresses(query)) {
			fwm::Row& row = addresses.add_row();
			row.cell(0).append(address->name());
			row.cell(1).append(address->to_string());
		}

		return addresses;
	}


	fwm::Table CliQueryAdrgCommand::do_query(const std::string & query)
	{
		fwm::Table groups({ "name", "value" }, { 0, 40 });

		for (const fos::PoolObject* pool : context.ostore.query_address_pools(query)) {
			fwm::Row& row = groups.add_row();
			row.cell(0).append(pool->name());
			row.cell(1).append(pool->to_string());
		}

		return groups;
	}


	fwm::Table CliQuerySvcCommand::do_query(const std::string & query)
	{
		fwm::Table services({ "name", "value" }, { 0, 40 });

		for (const fos::ServiceObject* service : context.ostore.query_services(query)) {
			fwm::Row& row = services.add_row();
			row.cell(0).append(service->name());
			row.cell(1).append(service->to_string());
		}

		return services;
	}


	fwm::Table CliQuerySvcgCommand::do_query(const std::string & query)
	{
		fwm::Table groups({ "name", "value" }, { 0, 40 });

		for (const fos::PoolObject* pool : context.ostore.query_service_pools(query)) {
			fwm::Row& row = groups.add_row();
			row.cell(0).append(pool->name());
			row.cell(1).append(pool->to_string());
		}

		return groups;
	}


	fwm::Table CliQueryAppCommand::do_query(const std::string & query)
	{
		fwm::Table applications({ "name", "value" }, { 0, 40 });

		for (const fos::ApplicationObject* application : context.ostore.query_application(query)) {
			fwm::Row& row = applications.add_row();
			row.cell(0).append(application->name());
			row.cell(1).append(application->to_string());
		}

		return applications;
	}


	fwm::Table CliQueryAppgCommand::do_query(const std::string & query)
	{
		fwm::Table groups({ "name", "value" }, { 0, 40 });

		for (const fos::PoolObject* pool : context.ostore.query_application_pools(query)) {
			fwm::Row& row = groups.add_row();
			row.cell(0).append(pool->name());
			row.cell(1).append(pool->to_string());
		}

		return groups;
	}


	fwm::Table CliQueryUsrCommand::do_query(const std::string & query)
	{
		fwm::Table users({ "name", "value" }, { 0, 40 });

		for (const fos::UserObject* user : context.ostore.query_user(query)) {
			fwm::Row& row = users.add_row();
			row.cell(0).append(user->name());
			row.cell(1).append(user->to_string());
		}

		return users;
	}


	fwm::Table CliQueryUsrgCommand::do_query(const std::string & query)
	{
		fwm::Table groups({ "name", "value" }, { 0, 40 });

		for (const fos::PoolObject* pool : context.ostore.query_user_pools(query)) {
			fwm::Row& row = groups.add_row();
			row.cell(0).append(pool->name());
			row.cell(1).append(pool->to_string());
		}

		return groups;
	}


	fwm::Table CliQueryUrlCommand::do_query(const std::string & query)
	{
		fwm::Table urls({ "url" }, { 0 });

		for (const fos::UrlObject* url : context.ostore.query_url(query)) {
			fwm::Row& row = urls.add_row();
			row.cell(0).append(url->name());
		}

		return urls;
	}


	fwm::Table CliQueryUrlgCommand::do_query(const std::string & query)
	{
		fwm::Table groups({ "name", "value" }, { 0, 40 });

		for (const fos::PoolObject* pool : context.ostore.query_url_pools(query)) {
			fwm::Row& row = groups.add_row();
			row.cell(0).append(pool->name());
			row.cell(1).append(pool->to_string());
		}

		return groups;
	}

}
