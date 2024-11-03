/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#pragma once
#include "global.h"

#include "tools/interrupt.h"


namespace cli {

	/* The class CliCtrlcHandler implements a Ctrl+C handler.
	*/
	class CliCtrlcHandler {
	public:
		CliCtrlcHandler();
		~CliCtrlcHandler();

		/* Enables the Ctrl+C handler.
		*/
		void enable();

		/* Disables the Ctrl+C handler.
		*/
		void disable();

		/* Returns true if Ctrl+C was pressed after the handler was enabled.
		*/
		bool is_interrupted() const;

	private:
		bool _enabled;
	};


	/* The class CliCtrlcGuard is a CtrlcHandler wrapper that provides a
	 * convenient RAII-style mechanism for enabling the CtrlC handler
	 * for the duration of a scoped block.  Only one instance of that
	 * class can be created.
	*/
	class CliCtrlcGuard {
	public:
		CliCtrlcGuard(CliCtrlcHandler& handler);
		~CliCtrlcGuard();

		/**
		 * Returns an interrupt function resolver_callback.
		 * A call to this function can be used to check if Ctrl+C was pressed.
		 * The returned function can not be used after the CliCtrlcGuard is
		 * deleted.
		*/
		f_interrupt_cb get_interrupt_cb() const;

	private:
		CliCtrlcHandler& _handler;
	};

}
