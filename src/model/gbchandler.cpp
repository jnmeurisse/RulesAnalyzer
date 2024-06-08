#include "gbchandler.h"

#include <cstdio>
#include "bdd.h"


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