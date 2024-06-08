/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#pragma once

#include <stdexcept>
#include <functional>

using f_interrupt_cb = std::function<bool()>;

class interrupt_error : public std::runtime_error
{
public:
	using runtime_error::runtime_error;
};
