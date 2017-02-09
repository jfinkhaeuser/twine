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
#ifndef TWINE_SCOPED_LOCK_H
#define TWINE_SCOPED_LOCK_H

#ifndef __cplusplus
#error You are trying to include a C++ only header file
#endif

#include <twine/twine.h>

#include <meta/stackonly.h>

#include <twine/mutex.h>

namespace twine {

/**
 * Forward declaration
 **/
namespace detail {

template <typename T, typename U>
struct unwrap_internals;

} // namespace detail

/**
 * Scoped lock for mutexes.
 *
 * Examples:
 * {
 *   mutex m;
 *   lock<mutex> lock(m);
 * }
 *
 * {
 *   recursive_mutex m;
 *   lock<recursive_mutex> lock(m);
 * }
 **/
template <
  typename mutexT
>
class scoped_lock : public meta::stackonly
{
public:
  scoped_lock(mutexT & mutex)
    : m_mutex(mutex)
  {
    m_mutex.lock();
  }

  ~scoped_lock()
  {
    m_mutex.unlock();
  }

  inline void lock()
  {
    m_mutex.lock();
  }

  inline bool try_lock()
  {
    return m_mutex.try_lock();
  }

  inline void unlock()
  {
    m_mutex.unlock();
  }


private:
  template <typename T, typename U>
  friend struct detail::unwrap_internals;

  mutexT & m_mutex;
};


} // namespace twine


#endif // guard

