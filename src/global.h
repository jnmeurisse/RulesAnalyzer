/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#pragma once

/*
 * Define Operating System (OS) based on compiler predefined macros.
 */
#if defined(__gnu_linux__)
#  define RA_OS_LINUX
# elif defined(_WIN64)
#  define RA_OS_WINDOWS
#else
#  error "Check the target OS !"
#endif


/*
 * Define the Target Architecture (TA)
 */
# if defined(__amd64__) || defined(__x86_64__) || defined(_M_X64) || defined(__AMD64__)
#  define RA_TA_AMD64
# elif defined(__i386__) || defined(_M_IX86) || defined(__X86__)
#  define RA_TA_X86
# elif defined(__arm__) || defined(__arm32__)
#  define RA_TA_ARM
# else
#  error "Check the target architecture !"
# endif


/*
 * Define if the architecture is little or big endian.
 */
#if defined(RA_TA_AMD64) || defined(RA_TA_X86) || defined(RT_TA_ARM)
# define RA_LITTLE_ENDIAN
#else
# error "Check the architecture !"
//# define RT_BIG_ENDIAN
#endif


#if defined(_MSC_VER)
# define NOMINMAX
#else
/* abstract class attribute is a specific MS keyword
*/
#define abstract

#endif
