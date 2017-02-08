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

#include <exception>
#include <stdexcept>

#include <meta/nullptr.h>

#include <twine/scoped_lock.h>
#include <twine/detail/thread_info.h>

namespace twine {


/******************************************************************************
 * Static members
 **/
thread::id const thread::bad_thread_id = -1;



/******************************************************************************
 * Implementation
 **/

thread::thread()
  : m_mutex()
  , m_info(nullptr)
  , m_is_attached(false)
  , m_handle(INVALID_HANDLE_VALUE)
{
}



thread::thread(thread::function func, void * baton,
    bool start_now /* = true */, bool detach_now /* = false */)
  : m_mutex()
  , m_info(nullptr)
  , m_is_attached(false)
  , m_handle(INVALID_HANDLE_VALUE)
{
  scoped_lock<recursive_mutex> lock(m_mutex);

  if (!set_func(func, baton)) {
    return;
  }

  if (start_now) {
    start(detach_now);
  }
}



thread::~thread()
{
  scoped_lock<recursive_mutex> lock(m_mutex);
  if (m_is_attached) {
    std::terminate();
  }
}



bool
thread::join()
{
  scoped_lock<recursive_mutex> lock(m_mutex);
  bool was_attached = m_is_attached;
  if (m_is_attached) {
    m_is_attached = false;
    lock.unlock();
    detail::thread_join(m_handle);
  }
  return was_attached;
}



bool
thread::joinable() const
{
  scoped_lock<recursive_mutex> lock(m_mutex);
  return m_is_attached;
}



void
thread::detach()
{
  scoped_lock<recursive_mutex> lock(m_mutex);
  if (m_is_attached) {
    detail::thread_detach(m_handle);

    // The current m_info structure is owned by the now-detached thread. This
    // means we can:
    // - Tell it not to do anything further with this thread object
    // - Sever this connection by copying m_info into a new structure
    thread_info * info = new thread_info(m_info);
    m_info->m_thread = nullptr;
    m_info = info;
    m_is_attached = false;
  }
}



bool
thread::set_func(function func, void * baton)
{
  // If we have no function object, we might as well exit immediately.
  if (!func) {
    return false;
  }

  scoped_lock<recursive_mutex> lock(m_mutex);

  // Check if this thread is currently attached. If so, we don't mess with it.
  if (m_is_attached) {
    return false;
  }

  // Set/overwrite thread info
  delete m_info;
  m_info = new thread_info(func, baton, this);
  return true;
}



void
thread::start(bool detach_now /* = false */)
{
  scoped_lock<recursive_mutex> lock(m_mutex);

  // Sanity checks
  if (!m_info || m_is_attached) {
    return;
  }

  // If we're supposed to detach immediately, we'll do so.
  thread_info * tmp_info = const_cast<thread_info *>(m_info);
  if (detach_now) {
    tmp_info = new thread_info(*tmp_info);
    tmp_info->m_thread = nullptr;
  }

  // Try to launch thread
  if (0 == detail::thread_create(m_handle, tmp_info))
  {
    if (detach_now) {
      detail::thread_detach(m_handle);
    }
    m_is_attached = !detach_now;
  }
}


thread::id
thread::get_id() const
{
  scoped_lock<recursive_mutex> lock(m_mutex);

  if (!m_info || !m_is_attached) {
    return bad_thread_id;
  }

  return m_info->m_id;
}



uint32_t
thread::hardware_concurrency()
{
#if defined(TWINE_HAVE_HW_NCPU)
  int data[] = { CTL_HW, HW_NCPU };
  int cpus = -1;
  size_t size = sizeof(cpus);
  int rc = ::sysctl(data, 2, &cpus, &size, NULL, 0);
  if (rc >= 0) {
    return cpus;
  }
  // fall through

#elif defined(TWINE_HAVE__SC_NPROCESSORS_ONLN)
  return ::sysconf(_SC_NPROCESSORS_ONLN);

#elif defined(TWINE_HAVE__SC_NPROC_ONLN)
  return ::sysconf(_SC_NPROC_ONLN);

#elif defined(TWINE_HAVE_GETSYSTEMINFO)
  // Windows
  SYSTEM_INFO sysinfo;
  GetSystemInfo(&sysinfo);
  return sysinfo.dwNumberOfProcessors;

#endif
  return 0;
}


/******************************************************************************
 * this_thread
 **/
namespace this_thread {

thread::id get_id()
{
  return detail::get_thread_id();
}

/**
 * Yield execution to another thread.
 **/
void yield()
{
#if defined(TWINE_HAVE_SCHED_YIELD)
  sched_yield();

#elif defined(TWINE_HAVE_SWITCHTOTHREAD)
  // Windows
  SwitchToThread();

#else
  throw std::runtime_error("yield() not implemented on this platform.");
#endif
}

} // namespace this_thread



} // namespace twine
