/**
 * This file is part of twine.
 *
 * Author(s): Jens Finkhaeuser <jens@unwesen.co.uk>
 *
 * Copyright (c) 2014 Unwesen Ltd.
 *
 * This software is licensed under the terms of the GNU GPLv3 for personal,
 * educational and non-profit use. For all other uses, alternative license
 * options are available. Please contact the copyright holder for additional
 * information, stating your intended usage.
 *
 * You can find the full text of the GPLv3 in the COPYING file in this code
 * distribution.
 *
 * This software is distributed on an "AS IS" BASIS, WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
 * PARTICULAR PURPOSE.
 **/
#ifndef TWINE_TWINE_H
#define TWINE_TWINE_H

#ifndef __cplusplus
#error You are trying to include a C++ only header file
#endif

#include <twine/twine-config.h>

#include <stdint.h>
#include <unistd.h>
#include <malloc.h>

// Which platform are we on?
#if !defined(TWINE_PLATFORM_DEFINED_)
  #if defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__)
    #define TWINE_WIN32
  #else
    #define TWINE_POSIX
  #endif
  #define TWINE_PLATFORM_DEFINED
#endif

// Check if we can support the assembly language level implementation (otherwise
// revert to the system API)
#if (defined(__GNUC__) && (defined(__i386__) || defined(__x86_64__))) || \
    (defined(_MSC_VER) && (defined(_M_IX86) || defined(_M_X64))) || \
    (defined(__GNUC__) && (defined(__ppc__)))
  #define TWINE_MUTEX_ASM
#else
  #define TWINE_MUTEX_SYS
#endif

// Decide what to include
#if defined(TWINE_WIN32)
  #ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
    #define __UNDEF_LEAN_AND_MEAN
  #endif
  #include <windows.h>
  #ifdef __UNDEF_LEAN_AND_MEAN
    #undef WIN32_LEAN_AND_MEAN
    #undef __UNDEF_LEAN_AND_MEAN
  #endif
#else
  #ifdef TWINE_MUTEX_ASM
    #include <sched.h>
  #else
    #include <pthread.h>
  #endif
#endif

#endif // guard
