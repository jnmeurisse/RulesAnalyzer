/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#include "tools/io.h"

#if defined(RA_OS_WINDOWS)
  #include <io.h>
  #include <errno.h>
  #define access _access
#else
  #include <unistd.h>
#endif


namespace rat {


	bool file_exists(const std::string& filename)
	{
		return access(filename.c_str(), 0) != -1;
	}

}
