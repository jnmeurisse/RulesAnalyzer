/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#include "cli/fw/clifw.h"
#include "cli/fw/clifwcheck.h"
#include "cli/fw/clifwcreate.h"
#include "cli/fw/clifwcopy.h"
#include "cli/fw/clifwdelete.h"
#include "cli/fw/clifwexport.h"
#include "cli/fw/clifwload.h"
#include "cli/fw/clifwoption.h"
#include "cli/fw/clifwshow.h"
#include "cli/fw/clifwselect.h"
#include "cli/fw/clifwinfo.h"


namespace cli {

	CliFwCommand::CliFwCommand(CliContext& context) :
		CliCommandMap(context)
	{
		add(CommandKeys{ "ch", "check" },   new CliFwCheckCommand(context));
		add(CommandKeys{ "cr", "create" },  new CliFwCreateCommand(context));
		add(CommandKeys{ "d", "delete" },   new CliFwDeleteCommand(context));
		add(CommandKeys{ "ex", "export" },  new CliFwExportCommand(context));
		add(CommandKeys{ "l", "load" },     new CliFwLoadCommand(context));
		add(CommandKeys{ "cp", "copy" },    new CliFwCopyCommand(context));
		add(CommandKeys{ "en", "enable" },  new CliFwEnableCommand(context));
		add(CommandKeys{ "di", "disable" }, new CliFwDisableCommand(context));
		add(CommandKeys{ "s", "select"},    new CliFwSelectCommand(context));
		add(CommandKeys{ "sh", "show" },    new CliFwShowCommand(context));
		add(CommandKeys{ "i", "info" },     new CliFwInfoCommand(context));
	}

}
