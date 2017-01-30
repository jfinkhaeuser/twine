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
#ifndef TWINE_TASKLET_H
#define TWINE_TASKLET_H

#ifndef __cplusplus
#error You are trying to include a C++ only header file
#endif

#include <twine/twine.h>

#include <twine/thread.h>
#include <twine/mutex.h>
#include <twine/condition.h>
#include <twine/chrono.h>
#include <twine/binder.h>

#include <meta/nullptr.h>


namespace twine {

/**
 * Tasklet class
 *
 * The tasklet class is a simple extension to the thread class even more focused
 * on reusability. In particular, it assumes that your typical threaded app will
 * run a number of threads for a long time, which wait on some condition varaible,
 * wake up, and then go to sleep again.
 *
 * That is, it simplifies this kind of thread function:
 *
 * scoped_lock lock(my_mutex);
 * while (running) {
 *   my_condition.timed_wait(lock, some_time);
 *   if (!running) {
 *     return;
 *  }
 *
 *  // Do stuff, e.g. pick up some work from a queue
 * }
 *
 * That means the tasklet encapsulates a flag that tells it whether it should
 * still be running, a condition variable, and a mutex.
 *
 * Quite often - e.g. when having multiple worker threads and a work queue -
 * multiple threads will want to share the same condition variable and mutex,
 * and that is also possible with the tasklet class.
 *
 * Note that in order for all of this to work, your thread function needs to
 * accept a tasklet reference. The actual implementation of such a tasklet
 * function would look like this:
 *
 * void my_func(tasklet & t) {
 *   while (tasklet.sleep(some_time)) {
 *     // do something
 *   }
 * }
 **/
class tasklet
  : public thread
{
public:
  /***************************************************************************
   * Typedefs
   **/
  // Typedef for the function type tasklets can run.
  typedef void (*function)(tasklet &, void *);

  // Binder
  template <
    typename classT,
    void (classT::*funcT)(tasklet &, void *)
  >
  struct binder : public twine::binder1<tasklet &, classT, funcT>
  {
  };

  /***************************************************************************
   * Constructor/destructor
   **/
  /**
   * Create a tasklet either with or without a condition. If created with
   * a condition, it shares the given condition, possibly with other tasklet
   * instances. Note that if you provide a shared condition, you will also
   * need to provide a shared recursive_mutex for waiting.
   *
   * Note that stop() and wakeup() will usually call notify_one() on an owned
   * condition object, such that the semantics of waking a tasklet out of
   * sleep() are met. For a shared condition object, that doesn't work quite
   * the same way; for that reason, the condition's notify_all() will be
   * called.
   *
   * If you don't want that, use the condition object you're passing directly.
   **/
  tasklet(function func, void * baton = nullptr, bool start_now = false);
  tasklet(twine::condition * condition, twine::recursive_mutex * mutex,
      function func, void * baton = nullptr, bool start_now = false);

  virtual ~tasklet();

  /***************************************************************************
   * Main interface
   **/
  /**
   * Overrides thread::start() in that detaching is no longer possible. Also
   * transitions the thread into running state. Returns true if the tasklet
   * was in a startable state and is now started.
   **/
  bool start();

  /**
   * Stops the internal thread by setting the thread into stopped state and
   * triggering the condition variable. Returns true if the tasklet was in
   * a stoppable state and is now stopped.
   **/
  bool stop();

  /**
   * Wait for the tasklet to terminate. This is synonymous to thread::join().
   **/
  bool wait();

  /**
   * Wakes the thread up from sleeping on the condition variable.
   **/
  void wakeup();

  /**
   * Sleep function. Use with twine::chrono's durations. Returns true if the
   * tasklet is still supposed to be running (i.e. sleep was interrupted by
   * wakeup() rather than stop()), so you can build your thread function like
   * this:
   *
   *   void func(tasklet & t, void * baton)
   *   {
   *     while (t.sleep()) {
   *       // Do something
   *     }
   *   }
   **/
  template <typename durationT>
  inline bool sleep(durationT const & duration) const
  {
    return tasklet::nanosleep(duration.template convert<twine::chrono::nanoseconds>());
  }

  inline bool sleep() const
  {
    return tasklet::nanosleep(twine::chrono::nanoseconds(-1));
  }


  /***************************************************************************
   * Forward declarations
   **/
  struct tasklet_info;

private:
  /***************************************************************************
   * Make stuff private that was public in thread
   **/
  bool joinable() const;
  void join();
  void detach();
  bool set_func(thread::function, void *);


  /***************************************************************************
   * Implementation functions
   **/
  bool nanosleep(twine::chrono::nanoseconds nsecs) const;

  /***************************************************************************
   * Data
   **/
  struct tasklet_info *             m_tasklet_info;

  volatile bool                     m_running;

  mutable twine::condition *        m_condition;
  mutable twine::recursive_mutex *  m_tasklet_mutex;
  bool                              m_condition_owned;
};

} // namespace twine


#endif // guard

