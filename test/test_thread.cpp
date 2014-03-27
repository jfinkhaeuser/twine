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

#include <twine/thread.h>


namespace {

struct baton
{
  int count;
  baton(int _count = 0)
    : count(_count)
  {
  }
};


void thread_incr(void * b)
{
  ++(static_cast<baton *>(b)->count);
}

void thread_decr(void * b)
{
  --(static_cast<baton *>(b)->count);
}

} // anonymous namespace

class ThreadTest
    : public CppUnit::TestFixture
{
public:
    CPPUNIT_TEST_SUITE(ThreadTest);

      CPPUNIT_TEST(testSingleThread);

    CPPUNIT_TEST_SUITE_END();

private:

    void testSingleThread()
    {
      // Creation auto-starts
      {
        baton b;
        twine::thread th(thread_incr, &b);
        CPPUNIT_ASSERT_EQUAL(true, th.joinable());
        th.join(); // XXX must do this or it all crashes
      }

      // Skip auto-start
      {
        baton b;
        twine::thread th(thread_incr, &b, false);
        CPPUNIT_ASSERT_EQUAL(false, th.joinable());
      }

      // Start and detach immediately
      {
        baton b;
        twine::thread th(thread_incr, &b, true, true);
        CPPUNIT_ASSERT_EQUAL(false, th.joinable());
      }

      // Create without func
      {
        baton b;
        twine::thread th;
        CPPUNIT_ASSERT_EQUAL(false, th.joinable());
      }

      // Create, add func later
      {
        baton b;
        twine::thread th;
        CPPUNIT_ASSERT_EQUAL(false, th.joinable());
        th.set_func(thread_incr, &b);
        CPPUNIT_ASSERT_EQUAL(false, th.joinable());
      }

      // Create, start later
      {
        baton b;
        twine::thread th;
        CPPUNIT_ASSERT_EQUAL(false, th.joinable());
        th.set_func(thread_incr, &b);
        CPPUNIT_ASSERT_EQUAL(false, th.joinable());
        th.start();
        CPPUNIT_ASSERT_EQUAL(true, th.joinable());
        th.join(); // XXX must do this or it all crashes
      }

      // Create, start & detach later
      {
        baton b;
        twine::thread th;
        CPPUNIT_ASSERT_EQUAL(false, th.joinable());
        th.set_func(thread_incr, &b);
        CPPUNIT_ASSERT_EQUAL(false, th.joinable());
        th.start(true);
        CPPUNIT_ASSERT_EQUAL(false, th.joinable());
      }
    }

};


CPPUNIT_TEST_SUITE_REGISTRATION(ThreadTest);
