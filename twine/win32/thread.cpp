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
#define TWINE_THREAD_DETAILS 1
#include <twine/thread.h>
#undef TWINE_THREAD_DETAILS

#include <process.h>
#include <errno.h>

#include <twine/detail/thread_info.h>
#include <twine/detail/thread_wrapper.tcc>

namespace twine {
namespace detail {

thread::id
get_thread_id()
{
  return ::GetCurrentThreadId();
}


void
thread_join(HANDLE & handle)
{
  if (INVALID_HANDLE_VALUE == handle) {
    return;
  }

  if (WAIT_OBJECT_0 == ::WaitForSingleObject(handle, INFINITE)) {
    ::CloseHandle(handle);
    handle = INVALID_HANDLE_VALUE;
  }
}


void
thread_detach(HANDLE & handle)
{
  ::CloseHandle(handle);
  handle = INVALID_HANDLE_VALUE;
}


int
thread_create(HANDLE & handle, thread::thread_info * info)
{
  handle = reinterpret_cast<HANDLE>(_beginthreadex(nullptr, 0,
        detail:: TWINE_ANONS(thread_wrapper), info, 0, nullptr));
  if (0 != handle) {
    return 0;
  }

  return errno;
}



} // namespace detail
} // namespace twine
