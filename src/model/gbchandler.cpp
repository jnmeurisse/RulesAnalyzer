/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#include "gbchandler.h"

#include <cstdio>
#include <buddy/bdd.h>


// Shows a 'G' when the garbage collector is running
static void analyze_gbc_hook(int pre, bddGbcStat* stat)
{
	if (pre == 1) {
		fputc('G', stdout);
		fflush(stdout);
	}
}

static bddgbchandler _old_handler = nullptr;


namespace fwm {

	GbcHandler::GbcHandler(bool show_activity)
	{
		_old_handler = bdd_gbc_hook(show_activity ? analyze_gbc_hook : nullptr);
	}


	GbcHandler::~GbcHandler()
	{
		bdd_gbc_hook(_old_handler);
		_old_handler = nullptr;
	}

}
