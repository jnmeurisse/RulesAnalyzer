/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#include "cli/clibdd.h"

#include <cstdio>
#include <buddy/bdd.h>


cli::CliBddCommand::CliBddCommand(CliContext& context) :
	CliCommandMap(context)
{
	add("info", new CliBddInfoCommand(context));
	add("gc", new CliBddGcCommand(context));
}


cli::CliBddInfoCommand::CliBddInfoCommand(CliContext& context) : 
	CliCommand(context, 0, 0, new CliCommandFlags())
{
}


void cli::CliBddInfoCommand::do_execute(CliArgs& args, const CliCtrlcGuard& ctrlc_guard)
{
	std::cout << std::string(bdd_versionstr()) << std::endl;

	// Print node statistics
	bddStat stat;
	bdd_stats(&stat);

	printf("\nNode statistics\n");
	printf("---------------\n");
	printf("maximum number of bdd nodes             : %d\n", stat.maxnodenum);
	printf("total number of new nodes ever produced : %zu\n", stat.produced);
	printf("currently allocated number of bdd nodes : %d\n", stat.nodenum);
	printf("number of currently free nodes          : %d\n", stat.freenodes);
	printf("minimum number of free nodes after gc   : %d\n", stat.minfreenodes);
	printf("number of bdd variables                 : %d\n", stat.varnum);
	printf("size of internal cache                  : %d\n", stat.cachesize);
	printf("number of garbage collections done      : %d\n", stat.gbcnum);

	// Print cache statistics
	bdd_printstat();
}


cli::CliBddGcCommand::CliBddGcCommand(CliContext& context) :
	CliCommand(context, 0, 0, new CliCommandFlags())
{
}


void cli::CliBddGcCommand::do_execute(CliArgs & args, const CliCtrlcGuard& ctrlc_guard)
{
	bdd_gbc();
}
