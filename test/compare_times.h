/**
 * This file is part of twine.
 *
 * Author(s): Jens Finkhaeuser <jens@finkhaeuser.de>
 *
 * Copyright (c) 2017 Jens Finkhaeuser.
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

#ifndef TWINE_TEST_COMPARE_TIMES_H
#define TWINE_TEST_COMPARE_TIMES_H

#include <cppunit/extensions/HelperMacros.h>

#include <twine/chrono.h>

template <typename firstT, typename secondT, typename expectedT>
inline void
compare_times(firstT const & first, secondT const & second, expectedT const & expected)
{
  namespace tc = twine::chrono;

  tc::nanoseconds diff = second.template convert<tc::nanoseconds>() - first.template convert<tc::nanoseconds>();

  CPPUNIT_ASSERT_MESSAGE("Cannot spend negative time!", diff.raw() > 0);

  // The diff should be no more than 25% off of the expected value, but also it's possible it will be >20ms due to scheduler granularity.
  tc::default_repr_t tmp = expected.template as<tc::nanoseconds>() * 1.25;
  tc::nanoseconds max = tc::nanoseconds(tmp);
  tc::nanoseconds twenty = tc::milliseconds(20).template convert<tc::nanoseconds>();
  if (max < twenty) {
    max = twenty;
  }

  CPPUNIT_ASSERT_MESSAGE("Should not fail except under high CPU workload or on emulators!", diff <= max);
}

#endif // guard
