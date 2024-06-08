/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#ifdef _WIN32
#include <io.h> 
#include <errno.h>
#define access _access
#else
#include <unistd.h>
#endif
#include "tools/io.h"

bool file_exists(const std::string& filename)
{
	return access(filename.c_str(), 0) != -1;
}
