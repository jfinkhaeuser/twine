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

#include <twine/tasklet.h>

namespace twine {

struct tasklet::tasklet_info
{
  tasklet *         m_tasklet;
  tasklet::function m_func;
  void *            m_thread_baton;

  tasklet_info(tasklet * tasklet, tasklet::function func, void * thread_baton)
    : m_tasklet(tasklet)
    , m_func(func)
    , m_thread_baton(thread_baton)
  {
  }
};


TWINE_ANONS_START

static void tasklet_wrapper(void * arg)
{
  tasklet::tasklet_info * info = static_cast<tasklet::tasklet_info *>(arg);
  info->m_func(*(info->m_tasklet), info->m_thread_baton);
}

TWINE_ANONS_END


tasklet::tasklet(twine::condition * condition, twine::recursive_mutex * mutex,
    tasklet::function func, void * baton /* = nullptr */, bool start_now /* = false */)
  : thread()
  , m_tasklet_info(new tasklet_info(this, func, baton))
  , m_running(false)
  , m_condition(condition)
  , m_tasklet_mutex(mutex)
  , m_condition_owned(false)
{
  thread::set_func(TWINE_ANONS(tasklet_wrapper), m_tasklet_info);
  if (start_now) {
    start();
  }
}



tasklet::tasklet(tasklet::function func, void * baton /* = nullptr */,
    bool start_now /* = false */)
  : thread()
  , m_tasklet_info(new tasklet_info(this, func, baton))
  , m_running(false)
  , m_condition(new twine::condition())
  , m_tasklet_mutex(&m_mutex)
  , m_condition_owned(true)
{
  thread::set_func(TWINE_ANONS(tasklet_wrapper), m_tasklet_info);
  if (start_now) {
    start();
  }
}



tasklet::~tasklet()
{
  stop();
  wait();
  delete m_tasklet_info;

  if (m_condition_owned) {
    delete m_condition;
  }
}



bool
tasklet::start()
{
  scoped_lock<recursive_mutex> lock(*m_tasklet_mutex);

  if (thread::joinable()) {
    return false;
  }

  m_running = true;
  thread::start();
  return true;
}



bool
tasklet::stop()
{
  scoped_lock<recursive_mutex> lock(*m_tasklet_mutex);

  if (!thread::joinable()) {
    return false;
  }

  m_running = false;

  if (m_condition_owned) {
    m_condition->notify_one();
  }
  else {
    m_condition->notify_all();
  }

  return true;
}



bool
tasklet::wait()
{
  // We ignore if it was joinable or not. That's because in the case of a tasklet,
  // you really want to wait for the tasklet to end.
  thread::join();
  return true;
}



void
tasklet::wakeup()
{
  if (m_condition_owned) {
    m_condition->notify_one();
  }
  else {
    m_condition->notify_all();
  }
}



bool
tasklet::nanosleep(twine::chrono::nanoseconds nsecs) const
{
  scoped_lock<recursive_mutex> lock(*m_tasklet_mutex);

  if (!m_running) {
    return false;
  }

  // Negative numbers mean sleep infinitely.
  if (nsecs < twine::chrono::nanoseconds(0)) {
    m_condition->wait(*m_tasklet_mutex);
    return m_running;
  }

  // Sleep for a given time period only.
  m_condition->timed_wait(*m_tasklet_mutex, twine::chrono::nanoseconds(nsecs));
  return m_running;
}


} // namespace twine
