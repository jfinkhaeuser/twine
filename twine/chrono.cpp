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
#include <twine/chrono.h>

#if defined(TWINE_HAVE_SYS_TIME_H) && defined(TWINE_HAVE_SYS_TYPES_H) && defined(TWINE_HAVE_UNISTD_H)
#  include <sys/time.h>
#  include <sys/types.h>
#  include <unistd.h>
#else
#  error Cannot compile on this system.
#endif

#include <meta/nullptr.h>

namespace twine {
namespace chrono {

nanoseconds now()
{
#if defined(TWINE_HAVE_SYS_TIME_H) && defined(TWINE_HAVE_SYS_TYPES_H) && defined(TWINE_HAVE_UNISTD_H)
  ::timeval tv;
  ::gettimeofday(&tv, nullptr);

  return nanoseconds(default_repr_t((tv.tv_sec * 1000000000) + (tv.tv_usec * 1000)));
#else
  return 0;
#endif
}


}} // namespace twine::chrono
