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
  , recursion_policyT()
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
  EnterCriticalSection(&m_handle);
  recursion_policyT::deadlock_multiple_threads();
}



template <
  typename recursion_policyT
>
bool
mutex_base<recursion_policyT>::try_lock()
{
  // First, try to enter the critical section. For recursive and non-recursive
  // mutexes, this succeeds if the current thread owns the handle.
  BOOL ret = TryEnterCriticalSection(&m_handle);
  if (!ret) {
    return false;
  }

  if (recursion_policyT::check_deadlock()) {
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
  recursion_policyT::lift_deadlock();
  LeaveCriticalSection(&m_handle);
}

} // namespace twine

#endif // guard
