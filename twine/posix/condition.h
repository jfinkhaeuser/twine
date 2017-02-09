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
#ifndef TWINE_POSIX_CONDITION_H
#define TWINE_POSIX_CONDITION_H

#ifndef __cplusplus
#error You are trying to include a C++ only header file
#endif

#include <twine/condition.h>

#if defined(TWINE_HAVE_TIME_H)
#  include <time.h>
#else
#  error POSIX threads require <time.h> for conditions
#endif

#include <errno.h>

#include <meta/nullptr.h>

#include <twine/detail/unwrap_internals.h>

namespace twine {

condition::condition()
  : m_handle()
{
  pthread_condattr_t attr;
  pthread_condattr_init(&attr);
  pthread_cond_init(&m_handle, &attr);
  pthread_condattr_destroy(&attr);
}



condition::~condition()
{
  pthread_cond_destroy(&m_handle);
}



template <typename lockableT>
void
condition::wait(lockableT & lockable)
{
  pthread_cond_wait(&m_handle,
      &detail::unwrap_internals<pthread_mutex_t, lockableT>::get_mutex_handle(lockable));
}



template <typename lockableT, typename durationT>
bool
condition::timed_wait(lockableT & lockable, durationT const & duration)
{
  chrono::nanoseconds delay = duration.template convert<chrono::nanoseconds>();
  delay += chrono::now();

  ::timespec wakeup;
  delay.as(wakeup);

  int ret = pthread_cond_timedwait(&m_handle,
      &detail::unwrap_internals<pthread_mutex_t, lockableT>::get_mutex_handle(lockable),
      &wakeup);
  return !(ret == ETIMEDOUT);
}



void
condition::notify_one()
{
  pthread_cond_signal(&m_handle);
}



void
condition::notify_all()
{
  pthread_cond_broadcast(&m_handle);
}

}

#endif // guard
