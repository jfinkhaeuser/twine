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
#ifndef TWINE_WIN32_MUTEX_POLICY_H
#define TWINE_WIN32_MUTEX_POLICY_H

#ifndef __cplusplus
#error You are trying to include a C++ only header file
#endif

#include <twine/twine.h>


namespace twine {
namespace detail {

/**
 * Policy for regular mutexes
 **/
struct nonrecursive_policy
{
  static inline void set_attributes(pthread_mutexattr_t * attr)
  {
// FIXME    pthread_mutexattr_settype(attr, PTHREAD_MUTEX_NORMAL);
  }
};



/**
 * Policy for recursive mutexes
 **/
struct recursive_policy
{
  static inline void set_attributes(pthread_mutexattr_t * attr)
  {
// FIXME    pthread_mutexattr_settype(attr, PTHREAD_MUTEX_RECURSIVE);
  }
};



}} // namespace twine::detail

#endif // guard
