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
#include <twine/thread.h>

#if defined(TWINE_HAVE_GETTID)
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>
#endif

#if defined(TWINE_HAVE_SCHED_YIELD)
#include <sched.h>
#endif

#if defined(TWINE_HAVE_PTHREAD_GETTHREADID_NP)
#include <pthread_np.h>
#endif

#include <exception>
#include <stdexcept>

#include <meta/nullptr.h>

#include <twine/scoped_lock.h>

namespace twine {

/******************************************************************************
 * Static members
 **/
thread::id const thread::bad_thread_id = -1;


/******************************************************************************
 * Helper functions
 **/
TWINE_ANONS_START

// Get a numeric thread ID.  We'll try to return an OS-specific handle.
static thread::id get_pthread_id()
{
#if defined(TWINE_HAVE_GETTID)
  return ::syscall(SYS_gettid);
#elif defined(TWINE_HAVE_PTHREAD_GETTHREADID_NP)
  return ::pthread_getthreadid_np();
#else
  return thread::bad_thread_id;
#endif
}

TWINE_ANONS_END


/**
 * Thread metadata.
 *
 * This is passed to the wrapper function and owned by the wrapper function.
 * However, what is passed is only a copy of the data owned by the thread
 * itself.
 *
 * This is necessary for detached threads - they need to be able to manage their
 * own metadata.
 **/
struct thread::thread_info
{
  thread::function    m_func;
  void *              m_baton;
  volatile thread *   m_thread;
  thread::id          m_id;

  thread_info(thread::function func, void * baton, thread * thread)
    : m_func(func)
    , m_baton(baton)
    , m_thread(thread)
    , m_id(bad_thread_id)
  {
  }

  thread_info(volatile thread_info const * other)
    : m_func(other->m_func)
    , m_baton(other->m_baton)
    , m_thread(other->m_thread)
    , m_id(bad_thread_id)
  {
  }

  inline bool equals(volatile thread_info const * other) const
  {
    thread_info const * tmp_info = const_cast<thread_info const *>(other);

    return (m_func == tmp_info->m_func
        && m_baton == tmp_info->m_baton
        && m_thread == tmp_info->m_thread);
  }

  inline void get_thread_id()
  {
    // If the thread object is a null pointer, we're already detached.
    thread * tmp_thread = const_cast<thread *>(m_thread);
    if (!tmp_thread) {
      return;
    }

    // XXX Possible, but unlikely race condition, see detach_from_thread_object()
    //     below.
    //
    //     tmp_thread may be destroyed already if it went out of scope
    //     immediately after start() was called. This would likely crash the
    //     program anyway (see thread dtor for details).
    scoped_lock<recursive_mutex> lock(tmp_thread->m_mutex);
    m_id = TWINE_ANONS(get_pthread_id)();
  }

  inline void detach_from_thread_object() const
  {
    // If the thread object is a null pointer, we're already detached
    thread * tmp_thread = const_cast<thread *>(m_thread);
    if (!tmp_thread) {
      return;
    }

    // XXX There's a possibility of a race condition here. If the thread object
    //     was attached and someone called thread::detach() in between the
    //     previous check and getting this lock, then we're hosed.
    //
    //     It's really hard to avoid this.
    scoped_lock<recursive_mutex> lock(tmp_thread->m_mutex);

    // Mark the thread object as detached only if we were not detached yet.
    // 1) If the thread object is attached to our current thread, then it:
    //    - has to be marked as attached
    //    - it's m_info must be equal to this
    //    -> detach the thread object
    // 2) If the thread object is detached from our current thead, then it:
    //    - must be marked as detached
    //    -> don't do anything
    // 3) If the thread object is attached to a different thead, then it:
    //  - has to be marked as attached
    //  - it's m_info must not be equal to this
    //    -> don't do anything
    if (tmp_thread->m_is_attached && equals(tmp_thread->m_info)) {
      tmp_thread->m_is_attached = false;
    }
  }
};



TWINE_ANONS_START


// Thread wrapper function - takes ownership of the passed info structure
static void * thread_wrapper(void * arg)
{
  thread::thread_info * info = static_cast<thread::thread_info *>(arg);

  // Get thead id.
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

  return nullptr;
}

TWINE_ANONS_END



/******************************************************************************
 * Implementation
 **/

thread::thread()
  : m_mutex()
  , m_info(nullptr)
  , m_is_attached(false)
  , m_handle()
{
}



thread::thread(thread::function func, void * baton,
    bool start_now /* = true */, bool detach_now /* = false */)
  : m_mutex()
  , m_info(nullptr)
  , m_is_attached(false)
  , m_handle()
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
    ::pthread_join(m_handle, nullptr);
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
    ::pthread_detach(m_handle);

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
  if (0 == ::pthread_create(&m_handle, nullptr,
        TWINE_ANONS(thread_wrapper), tmp_info))
  {
    if (detach_now) {
      ::pthread_detach(m_handle);
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
#if defined(TWINE_HAVE__SC_NPROCESSORS_ONLN)
  return ::sysconf(_SC_NPROCESSORS_ONLN);
#elif defined(TWINE_HAVE__SC_NPROC_ONLN)
  return ::sysconf(_SC_NPROC_ONLN);
#endif
  return 0;
}


/******************************************************************************
 * this_thread
 **/
namespace this_thread {

thread::id get_id()
{
  return TWINE_ANONS(get_pthread_id)();
}

/**
 * Yield execution to another thread.
 **/
void yield()
{
#if defined(TWINE_HAVE_SCHED_YIELD)
  sched_yield();
#else
  throw std::runtime_error("yield() not implemented on this platform.");
#endif
}

} // namespace this_thread



} // namespace twine
