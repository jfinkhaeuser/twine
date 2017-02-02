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
#ifndef TWINE_WIN32_MUTEX_H
#define TWINE_WIN32_MUTEX_H

#ifndef __cplusplus
#error You are trying to include a C++ only header file
#endif

#include <twine/twine.h>


namespace twine {

template <
  typename recursion_policyT
>
mutex_base<recursion_policyT>::mutex_base()
  : m_handle()
  , m_lock_count(0)
{
  InitializeCriticalSection(&m_handle);
}



template <
  typename recursion_policyT
>
mutex_base<recursion_policyT>::~mutex_base()
{
  DeleteCriticalSection(&m_handle);
}



template <
  typename recursion_policyT
>
void
mutex_base<recursion_policyT>::lock()
{
  while (true) {
    // First, try to enter the critical section. For recursive and non-recursive
    // mutexes, this succeeds if the current thread owns the handle.
    EnterCriticalSection(&m_handle);
    ++m_lock_count;

    // Perhaps the recursion policy disallows us entering recursively. In that
    // case, we leave again immediately. We busy-loop trying to stay in this
    // critical section.
    if (recursion_policyT::may_not_enter(m_lock_count)) {
      --m_lock_count;
      LeaveCriticalSection(&m_handle);
      continue;
    }

    break;
  }
}



template <
  typename recursion_policyT
>
bool
mutex_base<recursion_policyT>::try_lock()
{
  // First, try to enter the critical section. For recursive and non-recursive
  // mutexes, this succeeds if the current thread owns the handle.
  bool ret = TryEnterCriticalSection(&m_handle);
  if (!ret) {
    return false;
  }
  ++m_lock_count;

  // Perhaps the recursion policy disallows us entering recursively. In that
  // case, we leave again immediately. We busy-loop trying to stay in this
  // critical section.
  if (recursion_policyT::may_not_enter(m_lock_count)) {
    --m_lock_count;
    LeaveCriticalSection(&m_handle);
    return false;
  }

  return true;
}



template <
  typename recursion_policyT
>
void
mutex_base<recursion_policyT>::unlock()
{
  if (m_lock_count > 0) {
    --m_lock_count;
    LeaveCriticalSection(&m_handle);
  }
}

} // namespace twine

#endif // guard
