/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#include "cli/os/cliosload.h"

#include <string>
#include "tools/io.h"

namespace cli {
	CliOsLoadCommand::CliOsLoadCommand(CliContext& context) :
		CliCommandMap(context, 1, 2, nullptr)
	{
		add(CommandKeys{ "addr", "address" },           new CliLoadAdrCommand(context));
		add(CommandKeys{ "addrg", "address-group" },    new CliLoadAdrgCommand(context));
		add(CommandKeys{ "svc", "service"},             new CliLoadSvcCommand(context));
		add(CommandKeys{ "svcg", "service-group" },     new CliLoadSvcgCommand(context));
		add(CommandKeys{ "app", "application" },        new CliLoadAppCommand(context));
		add(CommandKeys{ "appg", "application-group" }, new CliLoadAppgCommand(context));
		add(CommandKeys{ "usr", "user" },               new CliLoadUsrCommand(context));
		add(CommandKeys{ "usrg", "user-group" },        new CliLoadUsrgCommand(context));
	}


	CliOsLoadSubCommand::CliOsLoadSubCommand(CliContext& context) :
		CliCommand(context, 1, 1, new CliCommandFlags())
	{
	};


	void CliOsLoadSubCommand::do_execute(CliArgs& args, const CliCtrlcGuard& ctrlc_guard)
	{
		const std::string filename{ args.pop() };

		if (!rat::file_exists(filename)) {
			context.logger->error("file '%s' not found", filename.c_str());
		}
		else {
			context.logger->info("loading objects from '%s'", filename.c_str());

			const int objects_counter = do_load_file(filename, ctrlc_guard.get_interrupt_cb());

			context.logger->info(
				"%d %s loaded",
				objects_counter,
				rat::pluralize(objects_counter, "object").c_str()
			);

			context.logger->flush();
		}
	};
}
