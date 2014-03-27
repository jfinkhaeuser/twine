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
#include <twine/twine.h>

#include <exception>

#include <meta/nullptr.h>

#include <twine/thread.h>
#include <twine/scoped_lock.h>

namespace twine {

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
  thread::function_type m_func;
  void *                m_baton;
  volatile thread *     m_thread;

  thread_info(thread::function_type func, void * baton, thread * thread)
    : m_func(func)
    , m_baton(baton)
    , m_thread(thread)
  {
  }

  thread_info(volatile thread_info const * other)
    : m_func(other->m_func)
    , m_baton(other->m_baton)
    , m_thread(other->m_thread)
  {
  }

  inline bool equals(volatile thread_info const * other) const
  {
    thread_info const * tmp_info = const_cast<thread_info const *>(other);

    return (m_func == tmp_info->m_func
        && m_baton == tmp_info->m_baton
        && m_thread == tmp_info->m_thread);
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



namespace {

// Thread wrapper function - takes ownership of the passed info structure
void * wrapper(void * arg)
{
  thread::thread_info * info = static_cast<thread::thread_info *>(arg);

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
}


} // anonymous namespace




thread::thread()
  : m_mutex()
  , m_info(nullptr)
  , m_is_attached(false)
  , m_handle()
{
}



thread::thread(thread::function_type func, void * baton,
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



void
thread::join()
{
  scoped_lock<recursive_mutex> lock(m_mutex);
  if (m_is_attached) {
    m_is_attached = false;
    lock.unlock();
    pthread_join(m_handle, nullptr);
  }
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
    pthread_detach(m_handle);

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
thread::set_func(function_type func, void * baton)
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
  if (0 == pthread_create(&m_handle, nullptr, wrapper, tmp_info)) {
    if (detach_now) {
      pthread_detach(m_handle);
    }
    m_is_attached = !detach_now;
  }
}

} // namespace twine
