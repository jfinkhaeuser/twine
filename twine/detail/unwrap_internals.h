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
#ifndef TWINE_DETAIL_UNWRAP_INTERNALS_H
#define TWINE_DETAIL_UNWRAP_INTERNALS_H

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
struct unwrap_internals
{
  inline static handleT & get_mutex_handle(lockableT &);
};

template <typename handleT>
struct unwrap_internals<handleT, mutex>
{
  inline static handleT & get_mutex_handle(mutex & mutex)
  {
    return mutex.m_handle;
  }
};

template <typename handleT>
struct unwrap_internals<handleT, recursive_mutex>
{
  inline static handleT & get_mutex_handle(recursive_mutex & mutex)
  {
    return mutex.m_handle;
  }
};

template <typename handleT>
struct unwrap_internals<handleT, scoped_lock<mutex> >
{
  inline static handleT & get_mutex_handle(scoped_lock<mutex> & lock)
  {
    return lock.m_mutex.m_handle;
  }
};

template <typename handleT>
struct unwrap_internals<handleT, scoped_lock<recursive_mutex> >
{
  inline static handleT & get_mutex_handle(scoped_lock<recursive_mutex> & lock)
  {
    return lock.m_mutex.m_handle;
  }
};

}} // namespace twine::detail

#endif // guard
