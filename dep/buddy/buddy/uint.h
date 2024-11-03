/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#pragma once
#include <stdint.h>

/* An unsigned 128 bit integer type.
*/
typedef struct _uint128 {
	uint64_t low64;
	uint64_t high64;
} s_uint128;
