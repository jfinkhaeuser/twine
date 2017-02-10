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
#ifndef TWINE_WIN32_CONDITION_H
#define TWINE_WIN32_CONDITION_H

#ifndef __cplusplus
#error You are trying to include a C++ only header file
#endif

#include <twine/condition.h>

#include <twine/detail/unwrap_internals.h>

namespace twine {

#define TWINE_CE_ONE 0
#define TWINE_CE_ALL 1

condition::condition()
  : m_waiters_lock()
  , m_waiters(0)
{
  m_events[TWINE_CE_ONE] = CreateEvent(nullptr, FALSE, FALSE, nullptr);
  m_events[TWINE_CE_ALL] = CreateEvent(nullptr, TRUE, FALSE, nullptr);
  InitializeCriticalSection(&m_waiters_lock);
}



condition::~condition()
{
  CloseHandle(m_events[TWINE_CE_ONE]);
  CloseHandle(m_events[TWINE_CE_ALL]);
  DeleteCriticalSection(&m_waiters_lock);
}



template <typename lockableT>
void
condition::wait(lockableT & lockable)
{
  condition::timed_wait(lockable, twine::chrono::milliseconds(INFINITE));
}



template <typename lockableT, typename durationT>
bool
condition::timed_wait(lockableT & lockable, durationT const & duration)
{
  // Increment waiters
  EnterCriticalSection(&m_waiters_lock);
  ++m_waiters;
  LeaveCriticalSection(&m_waiters_lock);

  // Release the lockable and wait for the condition.
  lockable.unlock();

  DWORD delay = static_cast<DWORD>(duration.as<twine::chrono::milliseconds>());
  int result = WaitForMultipleObjects(2, m_events, FALSE, delay);
  if (WAIT_TIMEOUT == result) {
    lockable.lock();
    return false;
  }

  EnterCriticalSection(&m_waiters_lock);
  --m_waiters;

  bool last_waiter = (WAIT_OBJECT_0 + TWINE_CE_ALL == result
      && 0 == m_waiters);
  LeaveCriticalSection(&m_waiters_lock);

  // If we are the last waiter, stop waiting.
  if (last_waiter) {
    ResetEvent(m_events[TWINE_CE_ALL]);
  }

  // Try to avoid resource starvation. If what happens after the condition
  // variable was signalled is fast, and in a loop, then the caller may just
  // enter the timed_wait() again before any other thread is scheduled.
  //
  // That's fine in theory, but in practice for the TWINE_CE_ALL event, it may
  // mean that m_waiters never goes to zero (goes to zero late), because it's
  // always the same thread incrementing and decrementing the counter.
  if (WAIT_OBJECT_0 + TWINE_CE_ALL == result) {
    this_thread::yield();
  }

  lockable.lock();

  return true;
}


void
condition::notify_internal(int event)
{
  EnterCriticalSection(&m_waiters_lock);
  bool have_waiters = (m_waiters > 0);
  LeaveCriticalSection(&m_waiters_lock);

  if (have_waiters) {
    SetEvent(m_events[event]);
  }
}



void
condition::notify_one()
{
  notify_internal(TWINE_CE_ONE);
}



void
condition::notify_all()
{
  notify_internal(TWINE_CE_ALL);
}

#undef TWINE_CE_ONE
#undef TWINE_CE_ALL

}

#endif // guard
