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
 * memory areas for several threads.
 *
 * On supported platforms/configurations, it is implemented as an atomic
 * spin lock with very low CPU overhead; elsewhere it uses system level
 * functions.
 *
 * FIXME
 **/
/// Fast mutex class.
/// This is a mutual exclusion object for synchronizing access to shared
/// memory areas for several threads. It is similar to the tthread::mutex class,
/// but instead of using system level functions, it is implemented as an atomic
/// spin lock with very low CPU overhead.
///
/// The \c fast_mutex class is NOT compatible with the \c condition_variable
/// class (however, it IS compatible with the \c lock_guard class). It should
/// also be noted that the \c fast_mutex class typically does not provide
/// as accurate thread scheduling as a the standard \c mutex class does.
///
/// Because of the limitations of the class, it should only be used in
/// situations where the mutex needs to be locked/unlocked very frequently.
///
/// @note The "fast" version of this class relies on inline assembler language,
/// which is currently only supported for 32/64-bit Intel x86/AMD64 and
/// PowerPC architectures on a limited number of compilers (GNU g++ and MS
/// Visual C++).
/// For other architectures/compilers, system functions are used instead.

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
  #include <twine/detail/mutex_win32.h>
  #include <twine/detail/mutex_policy_win32.h>
#elif defined(TWINE_POSIX)
  #include <twine/detail/mutex_posix.h>
  #include <twine/detail/mutex_policy_posix.h>
#endif

namespace twine {

typedef mutex_base<detail::nonrecursive_policy> mutex;
typedef mutex_base<detail::recursive_policy>    recursive_mutex;

} // namespace twine


#endif // guard

