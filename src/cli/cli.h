/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#pragma once

#include "ostore/ostoreconfig.h"

#include "cli/clicmdmap.h"
#include "cli/clicontext.h"

namespace cli {
	using namespace fwm;

	class Cli : public CliCommandMap
	{
	public:
		Cli(const OstoreConfig& config);

		/* Runs the command line interpreter
		*/
		int run();

	private:
		// The command line interface context 
		CliContext _context;

		/* Executes commands from the input stream
		*/
		void run(std::istream& in);

		/* Splits the input line in multiple arguments.  The arguments are pushed
		*  at the end of the `args` vector.  The number of arguments added to the
		*  vector is returned by the function.
		*/
		size_t split_args(const std::string& line, std::vector<std::string>& args);
	};
}
