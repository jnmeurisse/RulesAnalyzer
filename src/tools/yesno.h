/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#pragma once
#include "global.h"
#include <string>

namespace rat {

	bool question_yesno(const std::string& message, bool yes_default = true);

}
