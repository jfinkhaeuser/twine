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
#ifndef TWINE_DETAIL_UNWRAP_HANDLE_H
#define TWINE_DETAIL_UNWRAP_HANDLE_H

#ifndef __cplusplus
#error You are trying to include a C++ only header file
#endif

#include <twine/twine.h>

#include <twine/mutex.h>
#include <twine/scoped_lock.h>

namespace twine {
namespace detail {

/**
 * Helper construct for unwrapping the underlying raw mutex handle from a mutex
 * or lock object.
 **/
template <typename handleT, typename lockableT>
struct unwrap_handle
{
  inline static handleT & get(lockableT & lockable);
};

template <typename handleT>
struct unwrap_handle<handleT, mutex>
{
  inline static handleT & get(mutex & mutex)
  {
    return mutex.m_handle;
  }
};

template <typename handleT>
struct unwrap_handle<handleT, recursive_mutex>
{
  inline static handleT & get(recursive_mutex & mutex)
  {
    return mutex.m_handle;
  }
};

template <typename handleT>
struct unwrap_handle<handleT, scoped_lock<mutex> >
{
  inline static handleT & get(scoped_lock<mutex> & lock)
  {
    return lock.m_mutex.m_handle;
  }
};

template <typename handleT>
struct unwrap_handle<handleT, scoped_lock<recursive_mutex> >
{
  inline static handleT & get(scoped_lock<recursive_mutex> & lock)
  {
    return lock.m_mutex.m_handle;
  }
};

}} // namespace twine::detail

#endif // guard
