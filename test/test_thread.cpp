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

#include <time.h>

#include <twine/chrono.h>


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

void thread_sleep(void *)
{
  twine::this_thread::sleep_for(twine::chrono::milliseconds(100));
}

} // anonymous namespace

class ThreadTest
    : public CppUnit::TestFixture
{
public:
    CPPUNIT_TEST_SUITE(ThreadTest);

      CPPUNIT_TEST(testThisThread);
      CPPUNIT_TEST(testSingleThread);

    CPPUNIT_TEST_SUITE_END();

private:
    void testThisThread()
    {
      // Should get a thread ID
      CPPUNIT_ASSERT(twine::thread::bad_thread_id != twine::this_thread::get_id());

      // Yield should not throw.
      CPPUNIT_ASSERT_NO_THROW(twine::this_thread::yield());

      // Sleep
      time_t before = time(nullptr);
      twine::this_thread::sleep_for(twine::chrono::seconds(2));
      time_t after = time(nullptr);
      CPPUNIT_ASSERT(after > before);
    }



    void testSingleThread()
    {
      // Creation auto-starts
      {
        baton b;
        twine::thread th(thread_sleep, &b);
        CPPUNIT_ASSERT_EQUAL(true, th.joinable());
        twine::this_thread::sleep_for(twine::chrono::milliseconds(10));
        CPPUNIT_ASSERT(twine::thread::bad_thread_id != th.get_id());
        CPPUNIT_ASSERT(twine::this_thread::get_id() != th.get_id());

        th.join(); // XXX must do this or it all crashes
      }

      // Skip auto-start
      {
        baton b;
        twine::thread th(thread_sleep, &b, false);
        CPPUNIT_ASSERT_EQUAL(false, th.joinable());
        twine::this_thread::sleep_for(twine::chrono::milliseconds(10));
        CPPUNIT_ASSERT_EQUAL(twine::thread::bad_thread_id, th.get_id());
        CPPUNIT_ASSERT(twine::this_thread::get_id() != th.get_id());
      }

      // Start and detach immediately
      {
        baton b;
        twine::thread th(thread_sleep, &b, true, true);
        CPPUNIT_ASSERT_EQUAL(false, th.joinable());
        twine::this_thread::sleep_for(twine::chrono::milliseconds(10));
        CPPUNIT_ASSERT_EQUAL(twine::thread::bad_thread_id, th.get_id());
        CPPUNIT_ASSERT(twine::this_thread::get_id() != th.get_id());
      }

      // Create without func
      {
        baton b;
        twine::thread th;
        CPPUNIT_ASSERT_EQUAL(false, th.joinable());
        twine::this_thread::sleep_for(twine::chrono::milliseconds(10));
        CPPUNIT_ASSERT_EQUAL(twine::thread::bad_thread_id, th.get_id());
        CPPUNIT_ASSERT(twine::this_thread::get_id() != th.get_id());
      }

      // Create, add func later
      {
        baton b;
        twine::thread th;
        CPPUNIT_ASSERT_EQUAL(false, th.joinable());
        twine::this_thread::sleep_for(twine::chrono::milliseconds(10));
        CPPUNIT_ASSERT_EQUAL(twine::thread::bad_thread_id, th.get_id());
        CPPUNIT_ASSERT(twine::this_thread::get_id() != th.get_id());

        th.set_func(thread_sleep, &b);
        CPPUNIT_ASSERT_EQUAL(false, th.joinable());
        twine::this_thread::sleep_for(twine::chrono::milliseconds(10));
        CPPUNIT_ASSERT_EQUAL(twine::thread::bad_thread_id, th.get_id());
        CPPUNIT_ASSERT(twine::this_thread::get_id() != th.get_id());
      }

      // Create, start later
      {
        baton b;
        twine::thread th;
        CPPUNIT_ASSERT_EQUAL(false, th.joinable());
        twine::this_thread::sleep_for(twine::chrono::milliseconds(10));
        CPPUNIT_ASSERT_EQUAL(twine::thread::bad_thread_id, th.get_id());
        CPPUNIT_ASSERT(twine::this_thread::get_id() != th.get_id());

        th.set_func(thread_sleep, &b);
        CPPUNIT_ASSERT_EQUAL(false, th.joinable());
        twine::this_thread::sleep_for(twine::chrono::milliseconds(10));
        CPPUNIT_ASSERT_EQUAL(twine::thread::bad_thread_id, th.get_id());
        CPPUNIT_ASSERT(twine::this_thread::get_id() != th.get_id());

        th.start();
        CPPUNIT_ASSERT_EQUAL(true, th.joinable());
        twine::this_thread::sleep_for(twine::chrono::milliseconds(10));
        CPPUNIT_ASSERT(twine::thread::bad_thread_id != th.get_id());
        CPPUNIT_ASSERT(twine::this_thread::get_id() != th.get_id());

        th.join(); // XXX must do this or it all crashes
      }

      // Create, start & detach later
      {
        baton b;
        twine::thread th;
        CPPUNIT_ASSERT_EQUAL(false, th.joinable());
        CPPUNIT_ASSERT_EQUAL(false, th.joinable());
        twine::this_thread::sleep_for(twine::chrono::milliseconds(10));
        CPPUNIT_ASSERT_EQUAL(twine::thread::bad_thread_id, th.get_id());
        CPPUNIT_ASSERT(twine::this_thread::get_id() != th.get_id());

        th.set_func(thread_sleep, &b);
        CPPUNIT_ASSERT_EQUAL(false, th.joinable());
        CPPUNIT_ASSERT_EQUAL(false, th.joinable());
        twine::this_thread::sleep_for(twine::chrono::milliseconds(10));
        CPPUNIT_ASSERT_EQUAL(twine::thread::bad_thread_id, th.get_id());
        CPPUNIT_ASSERT(twine::this_thread::get_id() != th.get_id());

        th.start(true);
        CPPUNIT_ASSERT_EQUAL(false, th.joinable());
        CPPUNIT_ASSERT_EQUAL(false, th.joinable());
        twine::this_thread::sleep_for(twine::chrono::milliseconds(10));
        CPPUNIT_ASSERT_EQUAL(twine::thread::bad_thread_id, th.get_id());
        CPPUNIT_ASSERT(twine::this_thread::get_id() != th.get_id());
      }
    }

};


CPPUNIT_TEST_SUITE_REGISTRATION(ThreadTest);
