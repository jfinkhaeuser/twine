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
#ifndef TWINE_DETAIL_THREAD_INFO_H
#define TWINE_DETAIL_THREAD_INFO_H

#define TWINE_THREAD_DETAILS 1
#include <twine/thread.h>
#undef TWINE_THREAD_DETAILS

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
    m_id = detail::get_thread_id();
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


} // namespace twine

#endif // guard
