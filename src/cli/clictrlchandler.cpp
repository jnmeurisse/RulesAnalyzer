/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#include "cli/clictrlchandler.h"

#include <atomic>
#include <cstdio>
#include <stdexcept>

#if defined(RA_OS_WINDOWS)
   #include <Windows.h>
#elif defined(RA_OS_LINUX)
   #include <csignal>
#else
  // Ctrl-c not implemented
#endif

static std::atomic<bool> interrupted;

#if defined(RA_OS_WINDOWS)
static BOOL WINAPI HandlerRoutine(
	_In_ DWORD dwCtrlType
)
{
	switch (dwCtrlType)
	{
	case CTRL_C_EVENT:
		interrupted = true;
		return TRUE;

	default:
		break;
	}

	return FALSE;
}
#elif defined(RA_OS_LINUX)
static void sig_handler(int sig) {
	if (sig == SIGINT)
		interrupted = true;
}
#endif


namespace cli {

	CliCtrlcHandler::CliCtrlcHandler() :
		_enabled{ false }
	{
#if defined(RA_OS_WINDOWS)
		if (!::SetConsoleCtrlHandler(HandlerRoutine, TRUE))
			throw std::runtime_error("failed to set ctrl+c handler");
#elif defined(RA_OS_LINUX)
		std::signal(SIGINT, sig_handler);
#endif
	}


	CliCtrlcHandler::~CliCtrlcHandler()
	{
#if defined(RA_OS_WINDOWS)
		::SetConsoleCtrlHandler(HandlerRoutine, FALSE);
#elif defined(RA_OS_LINUX)
#endif
	}


	void CliCtrlcHandler::enable()
	{
		if (_enabled)
			throw std::runtime_error("failed to enable ctrl+c handler");

		interrupted = false;
		_enabled = true;
	}


	void CliCtrlcHandler::disable()
	{
		_enabled = false;
	}


	bool CliCtrlcHandler::is_interrupted() const
	{
		return _enabled && interrupted;
	}


	CliCtrlcGuard::CliCtrlcGuard(CliCtrlcHandler& handler)
		: _handler{ handler }
	{
		_handler.enable();
	}


	CliCtrlcGuard::~CliCtrlcGuard()
	{
		_handler.disable();
	}


	f_interrupt_cb CliCtrlcGuard::get_interrupt_cb() const
	{
		return [this]() -> bool { return this->_handler.is_interrupted(); };
	}

}
