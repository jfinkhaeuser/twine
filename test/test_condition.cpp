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

#include <cppunit/extensions/HelperMacros.h>

#include <twine/condition.h>
#include <twine/mutex.h>


class ConditionTest
    : public CppUnit::TestFixture
{
public:
    CPPUNIT_TEST_SUITE(ConditionTest);

      CPPUNIT_TEST(testConditionMutex);
      CPPUNIT_TEST(testConditionRecursiveMutex);

    CPPUNIT_TEST_SUITE_END();

private:

  void testConditionMutex()
  {
    // FIXME needs threads, so finish those
    CPPUNIT_FAIL("not yet implemented");
  }


  void testConditionRecursiveMutex()
  {
    // FIXME needs threads, so finish those
    CPPUNIT_FAIL("not yet implemented");
  }
};


CPPUNIT_TEST_SUITE_REGISTRATION(ConditionTest);
