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
#include <twine/chrono.h>

#if defined(TWINE_HAVE_SYS_TIME_H) && defined(TWINE_HAVE_SYS_TYPES_H) && defined(TWINE_HAVE_UNISTD_H)
#  include <sys/time.h>
#  include <sys/types.h>
#  include <unistd.h>
#else
#  error Cannot compile on this system.
#endif

#if defined(TWINE_HAVE_NANOSLEEP)
#  include <time.h>
#elif defined(TWINE_HAVE_SYS_SELECT_H)
#  include <sys/select.h>
#else
#  error Cannot compile on this system.
#endif

#include <errno.h>

#include <meta/nullptr.h>

namespace twine {
namespace chrono {

nanoseconds now()
{
#if defined(TWINE_HAVE_SYS_TIME_H) && defined(TWINE_HAVE_SYS_TYPES_H) && defined(TWINE_HAVE_UNISTD_H)

#if TWINE_HAVE_CLOCK_GETTIME
  ::timespec ts;
  ::clock_gettime(CLOCK_REALTIME, &ts);

  return nanoseconds(
    (default_repr_t(tv.tv_sec) * 1000000000)
    + (default_repr_t(tv.tv_nsec))
  );

#else // TWINE_HAVE_CLOCK_GETTIME
  ::timeval tv;
  ::gettimeofday(&tv, nullptr);

  return nanoseconds(
    (default_repr_t(tv.tv_sec) * 1000000000)
    + (default_repr_t(tv.tv_usec) * 1000)
  );
#endif // TWINE_HAVE_CLOCK_GETTIME

#else
#  error no implementation of now() available.
#endif
}



bool
sleep(nanoseconds const & nsec)
{
#if defined(TWINE_HAVE_NANOSLEEP)

  ::timespec spec;
  nsec.as(spec);

  while (-1 == ::nanosleep(&spec, &spec)) {
    switch (errno) {
      case EINTR:
        continue;

      case EFAULT:
      case EINVAL:
        return false; // XXX Can't really do anything here.
    }
  }
  return true;

#elif defined(TWINE_HAVE_SYS_SELECT_H)

  nanoseconds start = now();
  nanoseconds remain = nsec;
  do {
    ::timeval tv;
    remain.as(tv);

    int ret = ::select(0, nullptr, nullptr, nullptr, &tv);
    if (0 == ret) {
      break;
    }

    // Error handling really exists only because of EINTR - in which case we
    // may want to sleep again, for the remainder of the specified interval.
    if (-1 == ret) {
      switch (errno) {
        case EINTR:
          remain -= now() - start;
          break;

        case EINVAL:
        case ENOMEM:
        default:
          return false;
          break;
      }
    }

    // ret is neither 0 nor -1 - very unexpected.
    return false;

  } while (remain > nanoseconds(0));

  return true;
#else
#  error no implementation of sleep() available.
#endif
}


}} // namespace twine::chrono
