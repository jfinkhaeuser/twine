/**
 * This file is part of twine.
 *
 * Author(s): Jens Finkhaeuser <jens@finkhaeuser.de>
 *
 * Copyright (c) 2014 Unwesen Ltd.
 * Copyright (c) 2015-2017 Jens Finkhaeuser.
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
#include <stdlib.h>

#if defined(TWINE_HAVE_UNISTD_H)
#include <unistd.h>
#endif

/**
 * Which platform are we on?
 **/
#if !defined(TWINE_PLATFORM_DEFINED)
  #if defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__)
    #define TWINE_WIN32
  #else
    #define TWINE_POSIX
  #endif
  #define TWINE_PLATFORM_DEFINED
#endif

/**
 * Decide what to include globally
 **/
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
  #include <pthread.h>
#endif

// Make anonymous namespaces non-anonymous in debugging compilations.
#undef TWINE_ANONS_START
#undef TWINE_ANONS
#undef TWINE_ANONS_END
#if !defined(DEBUG) || defined(NDEBUG)
#  define TWINE_ANONS_START namespace {
#  define TWINE_ANONS(symbol) symbol
#  define TWINE_ANONS_END }
#else
#  define TWINE_ANONS_START namespace debug {
#  define TWINE_ANONS(symbol) debug:: symbol
#  define TWINE_ANONS_END }
#endif

#endif // guard
