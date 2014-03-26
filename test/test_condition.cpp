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
    twine::condition cond;
    twine::chrono::milliseconds wait(50);

    // Since we don't have several threads, we expect timed_wait() to time out
    {
      twine::mutex m;
      bool ret = cond.timed_wait(m, wait);
      CPPUNIT_ASSERT_EQUAL(false, ret);
    }

    // The same with a lock
    {
      twine::mutex m;
      twine::scoped_lock<twine::mutex> l(m);
      l.unlock();
      bool ret = cond.timed_wait(l, wait);
      CPPUNIT_ASSERT_EQUAL(false, ret);
    }

    // XXX We can't notify first and then wait, so we'll have to skip further
    //     tests here.
    CPPUNIT_FAIL("not fully implemented yet.");
  }


  void testConditionRecursiveMutex()
  {
    // XXX recursive mutexes (seemingly) need to be locked before we can 
    //     wait on them.

    twine::condition cond;
    twine::chrono::milliseconds wait(50);

    // Since we don't have several threads, we expect timed_wait() to time out
    {
      twine::recursive_mutex m;
      m.lock();
      bool ret = cond.timed_wait(m, wait);
      CPPUNIT_ASSERT_EQUAL(false, ret);
    }

    // The same with a lock
    {
      twine::recursive_mutex m;
      twine::scoped_lock<twine::recursive_mutex> l(m);
      bool ret = cond.timed_wait(l, wait);
      CPPUNIT_ASSERT_EQUAL(false, ret);
    }


    // XXX We can't notify first and then wait, so we'll have to skip further
    //     tests here.
    CPPUNIT_FAIL("not fully implemented yet.");
  }
};


CPPUNIT_TEST_SUITE_REGISTRATION(ConditionTest);
