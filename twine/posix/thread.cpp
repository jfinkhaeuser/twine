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

#if defined(TWINE_HAVE_GETTID)
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>
#endif

#if defined(TWINE_HAVE_HW_NCPU)
#include <sys/types.h>
#include <sys/sysctl.h>
#endif

#if defined(TWINE_HAVE_SCHED_YIELD)
#include <sched.h>
#endif

#if defined(TWINE_HAVE_PTHREAD_GETTHREADID_NP)
#include <pthread_np.h>
#endif

#include <twine/detail/thread_info.h>
#include <twine/detail/thread_wrapper.tcc>


namespace twine {
namespace detail {


// Get a numeric thread ID.  We'll try to return an OS-specific handle.
thread::id
get_thread_id()
{
#if defined(TWINE_HAVE_GETTID)
  return ::syscall(SYS_gettid);
#elif defined(TWINE_HAVE_PTHREAD_GETTHREADID_NP)
  return ::pthread_getthreadid_np();
#else
#error Cannot compile on this system; no known way to determine thread ID.
  return thread::bad_thread_id;
#endif
}


void
thread_join(pthread_t & handle)
{
  ::pthread_join(handle, nullptr);
  handle = INVALID_HANDLE_VALUE;
}


void
thread_detach(pthread_t & handle)
{
  ::pthread_detach(handle);
}


int
thread_create(HANDLE_T & handle, thread::thread_info * info)
{
  return ::pthread_create(&handle, nullptr, TWINE_ANONS(thread_wrapper), info);
}


} // namespace detail
} // namespace twine
