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
mutex_base::mutex_base()
  : m_handle()
  , m_already_locked(false)
{
  InitializeCriticalSection(&mHandle);
}



template <
  typename recursion_policyT
>
mutex_base::~mutex_base()
{
  DeleteCriticalSection(&m_handle);
}



template <
  typename recursion_policyT
>
void
mutex_base::lock()
{
  // FIXME loop over m_already_locked? must be better stuff in Joost code base
  EnterCriticalSection(&m_handle);
}



template <
  typename recursion_policyT
>
bool
mutex_base::try_lock()
{
  bool ret = TryEnterCriticalSection(&m_handle);
  if (ret && m_already_locked) {
    // FIXME this can't be right! this _unlocks_ an already locked
    // mutex?
    LeaveCriticalSection(&m_handle);
    ret = false;
  }
  return ret;
}



template <
  typename recursion_policyT
>
void
mutex_base::unlock()
{
  m_already_locked = false;
  LeaveCriticalSection(&m_handle);
}

} // namespace twine

#endif // guard
