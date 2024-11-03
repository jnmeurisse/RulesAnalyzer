/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#pragma once

#if __cplusplus < 201703L
    #include "tools/optional.hpp"

	namespace std {
		template <class T>
		using optional = std::experimental::optional<T>;
	}

#else
    #include <optional>

#endif
