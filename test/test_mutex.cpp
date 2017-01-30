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

#include <cppunit/extensions/HelperMacros.h>

#include <twine/mutex.h>


class MutexTest
    : public CppUnit::TestFixture
{
public:
    CPPUNIT_TEST_SUITE(MutexTest);

      CPPUNIT_TEST(testMutex);
      CPPUNIT_TEST(testRecursiveMutex);

    CPPUNIT_TEST_SUITE_END();

private:

  void testMutex()
  {
    // Test ctor/dtor
    CPPUNIT_ASSERT_NO_THROW( twine::mutex m; );

    // Test locking
    {
      twine::mutex m;
      m.lock();
      CPPUNIT_ASSERT_EQUAL(false, m.try_lock());
      m.unlock();
      CPPUNIT_ASSERT_EQUAL(true, m.try_lock());
      m.unlock();
    }

    // Test locked destruction
    {
      twine::mutex m;
      m.lock();
    }
  }


  void testRecursiveMutex()
  {
    // Test ctor/dtor
    CPPUNIT_ASSERT_NO_THROW( twine::recursive_mutex m; );

    // Test locking
    {
      twine::recursive_mutex m;
      CPPUNIT_ASSERT_EQUAL(true, m.try_lock());
      CPPUNIT_ASSERT_EQUAL(true, m.try_lock());
      m.unlock();
    }

    // Test locked destruction
    {
      twine::recursive_mutex m;
      m.lock();
    }
  }
};


CPPUNIT_TEST_SUITE_REGISTRATION(MutexTest);
