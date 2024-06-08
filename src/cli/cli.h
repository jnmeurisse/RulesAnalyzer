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

		/*
		*/
		size_t split_args(const std::string& line, std::vector<std::string>& tokens);
	};
}
