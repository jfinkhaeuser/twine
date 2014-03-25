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
#ifndef TWINE_MUTEX_H
#define TWINE_MUTEX_H

#ifndef __cplusplus
#error You are trying to include a C++ only header file
#endif

#include <twine/twine.h>


namespace twine {

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
{
public:
  // Constructor/destructor
  mutex_base();
  ~mutex_base();

  // Lock the mutex.
  inline void lock();
  inline bool try_lock();

  // Unlock the mutex
  inline void unlock();

private:
  mutex_base(mutex_base const &) = delete;
  mutex_base & operator=(mutex_base const &) = delete;

#if defined(TWINE_WIN32)
  CRITICAL_SECTION  m_handle;
  volatile bool     m_already_locked;
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

