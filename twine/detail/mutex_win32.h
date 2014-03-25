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
#ifndef TWINE_DETAIL_MUTEX_WIN32_H
#define TWINE_DETAIL_MUTEX_WIN32_H

#ifndef __cplusplus
#error You are trying to include a C++ only header file
#endif

#include <twine/twine.h>


namespace twine {

mutex_base::mutex_base()
  : m_handle()
{
  InitializeCriticalSection(&mHandle);
}



mutex_base::~mutex_base()
{
  DeleteCriticalSection(&m_handle);
}



void
mutex_base::lock()
{
  EnterCriticalSection(&m_handle);
}



bool
mutex_base::try_lock()
{
  return TryEnterCriticalSection(&m_handle);
}



void
mutex_base::unlock()
{
  LeaveCriticalSection(&m_handle);
}

} // namespace twine

#endif // guard
