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
#ifndef TWINE_POSIX_CONDITION_H
#define TWINE_POSIX_CONDITION_H

#ifndef __cplusplus
#error You are trying to include a C++ only header file
#endif

#include <twine/twine.h>

#include <sys/time.h>

#include <twine/detail/unwrap_handle.h>

namespace twine {

condition::condition()
  : m_handle(PTHREAD_COND_INITIALIZER)
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
      &detail::unwrap_handle<pthread_mutex_t, lockableT>::get());
}



template <typename lockableT, typename durationT>
bool
condition::timed_wait(lockableT & lockable, durationT const & duration)
{
  ::timeval now;
  ::gettimeofday(&now, NULL);

  ::timespec wakeup;
  duration.as(wakeup);

  wakeup.tv_sec += now.tv_sec;
  wakeup.tv_nsec += now.tv_usec * 1000;
  while (wakeup.tv_nsec > 1000000000) {
    ++wakeup.tv_sec;
    wakeup.tv_nsec -= 1000000000;
  }

  int ret = pthread_cond_timedwait(&m_handle,
      &detail::unwrap_handle<pthread_mutex_t, lockableT>::get(lockable),
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
