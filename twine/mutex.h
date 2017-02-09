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
#ifndef TWINE_MUTEX_H
#define TWINE_MUTEX_H

#ifndef __cplusplus
#error You are trying to include a C++ only header file
#endif

#include <twine/twine.h>

#include <twine/noncopyable.h>

namespace twine {

/**
 * Forward declaration
 **/
namespace detail {

template <typename T, typename U>
struct unwrap_internals;

} // namespace detail

/**
 * Mutex class
 *
 * This is a mutual exclusion object for synchronizing access to shared
 * memory areas for several threads. It's an abstraction over system
 * provided mutexes.
 *
 * There's a variant for recursive and a variant for non-recursive mutexes
 * called recursive_mutex and mutex respectively.
 **/
template <
  typename recursion_policyT
>
class mutex_base
  : public twine::noncopyable
  , private recursion_policyT
{
public:
  typedef recursion_policyT recursion_policy_t;

  // Constructor/destructor
  mutex_base();
  ~mutex_base();

  // Lock the mutex.
  inline void lock();
  inline bool try_lock();

  // Unlock the mutex
  inline void unlock();

private:
  template <typename T, typename U>
  friend struct detail::unwrap_internals;

#if defined(TWINE_WIN32)
  CRITICAL_SECTION  m_handle;
#elif defined(TWINE_POSIX)
  pthread_mutex_t   m_handle;
#endif
};

} // namespace twine


#if defined(TWINE_WIN32)
  #include <twine/win32/mutex.h>
  #include <twine/win32/mutex_policy.h>
#elif defined(TWINE_POSIX)
  #include <twine/posix/mutex.h>
  #include <twine/posix/mutex_policy.h>
#endif

namespace twine {

typedef mutex_base<detail::nonrecursive_policy> mutex;
typedef mutex_base<detail::recursive_policy>    recursive_mutex;

} // namespace twine


#endif // guard

