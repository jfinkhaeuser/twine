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
#ifndef TWINE_POSIX_MUTEX_H
#define TWINE_POSIX_MUTEX_H

#ifndef __cplusplus
#error You are trying to include a C++ only header file
#endif

#include <twine/twine.h>


namespace twine {

template <
  typename recursion_policyT
>
mutex_base<recursion_policyT>::mutex_base()
  : m_handle()
{
  pthread_mutexattr_t attr;
  pthread_mutexattr_init(&attr);
  recursion_policyT::set_attributes(&attr);
  pthread_mutex_init(&m_handle, &attr);
  pthread_mutexattr_destroy(&attr);
}



template <
  typename recursion_policyT
>
mutex_base<recursion_policyT>::~mutex_base()
{
  pthread_mutex_destroy(&m_handle);
}



template <
  typename recursion_policyT
>
void
mutex_base<recursion_policyT>::lock()
{
  pthread_mutex_lock(&m_handle);
}



template <
  typename recursion_policyT
>
bool
mutex_base<recursion_policyT>::try_lock()
{
  return (0 == pthread_mutex_trylock(&m_handle));
}



template <
  typename recursion_policyT
>
void
mutex_base<recursion_policyT>::unlock()
{
  pthread_mutex_unlock(&m_handle);
}

} // namespace twine

#endif // guard
