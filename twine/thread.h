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
#ifndef TWINE_THREAD_H
#define TWINE_THREAD_H

#ifndef __cplusplus
#error You are trying to include a C++ only header file
#endif

#include <twine/twine.h>

#include <twine/noncopyable.h>

#include <twine/mutex.h>
#include <twine/chrono.h>
#include <twine/binder.h>

#include <meta/nullptr.h>

namespace twine {

/***************************************************************************
 * Forward declarations
 **/
class tasklet;

/**
 * Thread class
 *
 * This thread class goes a little beyond the standard thread classes in that
 * thread objects are re-usable. First, understand that thread objects - this
 * or from other libraries - are attached to a thread of execution, and it's
 * usually possible to detach() them.
 *
 * Once detached, the thread object can be re-used. Calling start() will start
 * a *new* thread of execution with the same thread function and baton. If you
 * want to use a new thread function and baton, set them via set_func().
 *
 * For reasons of staying mostly compatible with other thread library
 * implementations, joinable() returns true of the object is attached to a
 * thread of execution. For reasons of avoiding horrible race conditions, you
 * should probably just try to join() or detach() a thread object instead of
 * checking that flag, though - both functions don't do anything if the thread
 * object is not joinable at the moment.
 *
 * A thread object that is joinable() and goes out of scope will terminate the
 * program immediately. That sucks, but the alternatives may be worse:
 * - trying to join() automatically may block the program indefinitely
 * - trying to detach() automatically may have unintended side effects
 * - throwing exceptions in a destructor is bad.
 *
 * At least this way it'll fail loudly.
 **/
class thread
  : public twine::noncopyable
{
public:
  /***************************************************************************
   * Typedefs
   **/
  // Typedef for the function type threads can run.
  typedef void (*function)(void *);

  // Binder
  template <
    typename classT,
    void (classT::*funcT)(void *)
  >
  struct binder : public twine::binder0<classT, funcT>
  {
  };

  // Thread ID
  typedef int64_t id;

  /***************************************************************************
   * Constructor/destructor
   **/
  thread();
  thread(function func, void * baton, bool start_now = true,
      bool detach_now = false);

  virtual ~thread();

  /***************************************************************************
   * Main interface
   **/
  /**
   * A thread is joinable if and only if it currently runs a thread function.
   * If the function terminates, it is no longer joinable.
   **/
  bool joinable() const;

  /**
   * Join a thread. This is a no-op on non-joinable objects. Returns true if
   * the thread was joined, false otherwise.
   **/
  bool join();

  /**
   * Detach the thread from this thread object. The thread object becomes
   * non-joinable as a result.
   *
   * There's a potential race condition if you're calling detach() close to
   * the time the started thread ends naturally. For this reason, the start()
   * function accepts an optional detach_now parameter (see below) that avoids
   * this race.
   **/
  void detach();

  /**
   * Get the thread ID; every thread has one. Returns bad_thread_id if the
   * thread object is not currently attached to an execution thread.
   **/
  static id const bad_thread_id;
  id get_id() const;

  /**
   * Return the number of threads the hardware can run concurrently. The
   * standard requires that zero is returned if the number cannot be
   * determined.
   **/
  static uint32_t hardware_concurrency();


  /***************************************************************************
   * Extended functionality
   **/
  /**
   * Thread objects that are non-joinable can be turned into joinable thread
   * objects by setting a thread function and calling start() below. Note that
   * set_func() fails (returns false) if the thread object is currently joinable.
   **/
  bool set_func(function func, void * baton);

  /**
   * If the thread was constructed with start_now = false, or was joined and
   * made joinable again via set_func, then you can start the thread via this
   * start() function. It's a no-op if its preconditions are not met.
   *
   * See detach() for a description on the detach_now parameter.
   **/
  void start(bool detach_now = false);


  /***************************************************************************
   * Forward declarations
   **/
  struct thread_info;

private:
  friend struct thread_info;

  friend class tasklet;

  // Thread data
  mutable recursive_mutex       m_mutex;
  volatile struct thread_info * m_info;
  volatile bool                 m_is_attached;

#if defined(TWINE_WIN32)
  // TODO
#elif defined(TWINE_POSIX)
  pthread_t   m_handle;
#endif

};


namespace this_thread {

/**
 * Same as thread::get_id() except returns thread ID of the calling thread.
 **/
thread::id get_id();

/**
 * Yield execution to another thread. Throws std::runtime_error() if yielding
 * is not possible on the given platform.
 **/
void yield();

/**
 * Put the calling thread to sleep for the duration given in the period. Returns
 * false on unexpected errors, true otherwise. Note that sleep_for() will ignore
 * signals.
 **/
template <typename periodT>
bool
sleep_for(periodT const & period)
{
  return chrono::sleep(period.template convert<chrono::nanoseconds>());
}

} // namespace this_thread

} // namespace twine


#endif // guard

