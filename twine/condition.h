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
#ifndef TWINE_CONDITION_H
#define TWINE_CONDITION_H

#ifndef __cplusplus
#error You are trying to include a C++ only header file
#endif

#include <twine/twine.h>

#include <twine/mutex.h>

namespace twine {

/**
 * Condition variable.
 *
 * Example:
 *
 * void wait_counter(int targetCount)
 * {
 *   guard<mutex> guard(m);
 *   while(count < targetCount)
 *     cond.wait(m);
 * }
 *
 * void increment()
 * {
 *   guard<mutex> guard(m);
 *   ++count;
 *   cond.notify_all();
 * }
 **/
class condition
{
public:
  // Constructor/destructor
  condition();
  ~condition();

  // Main interface
  template <typename mutexT>
  inline void wait(mutexT & mutex);

  // Notify
  inline void notify_one();
  inline void notify_all();

private:
#if defined(TWINE_WIN32)
  // TODO
#elif defined(TWINE_POSIX)
  pthread_cond_t  m_handle;
#endif
};


} // namespace twine

#if defined(TWINE_WIN32)
  #include <twine/detail/condition_win32.h>
#elif defined(TWINE_POSIX)
  #include <twine/detail/condition_posix.h>
#endif

#endif // guard

