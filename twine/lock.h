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
#ifndef TWINE_LOCK_H
#define TWINE_LOCK_H

#ifndef __cplusplus
#error You are trying to include a C++ only header file
#endif

#include <twine/twine.h>

#include <twine/mutex.h>

namespace twine {

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
class lock
{
public:
  lock(mutexT & mutex)
    : m_mutex(mutex)
  {
    m_mutex.lock();
  }

  ~lock()
  {
    m_mutex.unlock();
  }

private:
  mutexT & m_mutex;
};


} // namespace twine


#endif // guard

