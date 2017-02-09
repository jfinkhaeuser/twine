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
#ifndef TWINE_CONDITION_H
#define TWINE_CONDITION_H

#ifndef __cplusplus
#error You are trying to include a C++ only header file
#endif

#include <twine/twine.h>

#include <twine/mutex.h>
#include <twine/scoped_lock.h>
#include <twine/chrono.h>

namespace twine {

/**
 * Condition variable.
 *
 * Example:
 *
 * void wait_counter(int targetCount)
 * {
 *   guard<mutex> guard(m);
 *   while(count < targetCount)
 *     cond.wait(m);
 * }
 *
 * void increment()
 * {
 *   guard<mutex> guard(m);
 *   ++count;
 *   cond.notify_all();
 * }
 **/
class condition
{
public:
  // Constructor/destructor
  inline condition();
  inline ~condition();

  // Main interface. Use with mutexes or scoped locks, and durations
  // defined in chrono.h. The lockable should be locked before passing it to
  // either of the wait functions.
  template <typename lockableT>
  inline void wait(lockableT & lockable);

  template <typename lockableT, typename durationT>
  inline bool timed_wait(lockableT & lockable, durationT const & duration);

  // Notify waiting threads
  inline void notify_one();
  inline void notify_all();

private:
#if defined(TWINE_WIN32)
  inline void notify_internal(int event);

  CRITICAL_SECTION      m_waiters_lock;
  volatile unsigned int m_waiters;
  HANDLE                m_events[2];
#elif defined(TWINE_POSIX)
  pthread_cond_t        m_handle;
#endif
};


} // namespace twine

#if defined(TWINE_WIN32)
  #include <twine/win32/condition.h>
#elif defined(TWINE_POSIX)
  #include <twine/posix/condition.h>
#endif

#endif // guard

