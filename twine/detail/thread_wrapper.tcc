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
#ifndef TWINE_DETAIL_THREAD_WRAPPER_TCC
#define TWINE_DETAIL_THREAD_WRAPPER_TCC

#ifndef __cplusplus
#error You are trying to include a C++ only header file
#endif

#include <twine/twine.h>

namespace twine {
namespace detail {

TWINE_ANONS_START

// Thread wrapper function - takes ownership of the passed info structure
#ifdef TWINE_WIN32
static unsigned __stdcall thread_wrapper(void * arg)
#define TWINE_THREAD_WRAPPER_RETVAL 0;
#else // TWINE_WIN32
static void * thread_wrapper(void * arg)
#define TWINE_THREAD_WRAPPER_RETVAL nullptr;
#endif // TWINE_WIN32
{
  thread::thread_info * info = static_cast<thread::thread_info *>(arg);

  // Get thread id.
  info->get_thread_id();

  // Run thread function safely - terminate the thread on any exception
  try {
    info->m_func(info->m_baton);
  } catch (...) {
    delete info;
    std::terminate();
  }

  // Detach the current thread of execution from the thread object held in the
  // info structure.
  info->detach_from_thread_object();

  // Cleanup
  delete info;

  return TWINE_THREAD_WRAPPER_RETVAL;
}
#undef TWINE_THREAD_WRAPPER_RETVAL

TWINE_ANONS_END

} // namespace detail
} // namespace twine

#endif // guard
