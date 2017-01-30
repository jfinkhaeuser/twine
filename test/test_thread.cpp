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

#include <twine/thread.h>

#include <sys/time.h>

#include <twine/chrono.h>
#include <twine/scoped_lock.h>

#define THREAD_TEST_SHORT_DELAY twine::chrono::milliseconds(1)
#define THREAD_TEST_LONG_DELAY  twine::chrono::milliseconds(100)

namespace {

struct baton
{
  int count;
  twine::mutex m;
  baton(int _count = 0)
    : count(_count)
    , m()
  {
  }
};


void thread_incr(void * arg)
{
  baton * b = static_cast<baton *>(arg);
  twine::scoped_lock<twine::mutex> lock(b->m);
  ++(b->count);
}

void thread_decr(void * arg)
{
  baton * b = static_cast<baton *>(arg);
  twine::scoped_lock<twine::mutex> lock(b->m);
  --(b->count);
}

void thread_sleep(void *)
{
  twine::this_thread::sleep_for(THREAD_TEST_LONG_DELAY);
}

struct bind_test
{
  bool called;

  bind_test()
    : called(false)
  {
  }

  void member(void * baton)
  {
    CPPUNIT_ASSERT_EQUAL(static_cast<void*>(this), baton);
    twine::this_thread::sleep_for(THREAD_TEST_LONG_DELAY);
    called = true;
  }
};

} // anonymous namespace

class ThreadTest
    : public CppUnit::TestFixture
{
public:
    CPPUNIT_TEST_SUITE(ThreadTest);

      CPPUNIT_TEST(testThisThread);
      CPPUNIT_TEST(testSingleThread);
      CPPUNIT_TEST(testMultipleThreads);
      CPPUNIT_TEST(testBinder);
      CPPUNIT_TEST(testHardwareConcurrency);

    CPPUNIT_TEST_SUITE_END();

private:
    void testThisThread()
    {
      // Should get a thread ID
      CPPUNIT_ASSERT(twine::thread::bad_thread_id != twine::this_thread::get_id());

      // Yield should not throw.
      CPPUNIT_ASSERT_NO_THROW(twine::this_thread::yield());

      // Sleep
      ::timeval before;
      ::gettimeofday(&before, nullptr);
      twine::this_thread::sleep_for(THREAD_TEST_LONG_DELAY);
      ::timeval after;
      ::gettimeofday(&after, nullptr);
      CPPUNIT_ASSERT(after.tv_sec > before.tv_usec
          || (after.tv_sec == before.tv_sec && after.tv_usec > before.tv_usec));
    }



    void testSingleThread()
    {
      // Creation auto-starts
      {
        baton b;
        twine::thread th(thread_sleep, &b);
        CPPUNIT_ASSERT_EQUAL(true, th.joinable());
        twine::this_thread::sleep_for(THREAD_TEST_SHORT_DELAY);
        CPPUNIT_ASSERT(twine::thread::bad_thread_id != th.get_id());
        CPPUNIT_ASSERT(twine::this_thread::get_id() != th.get_id());

        twine::this_thread::sleep_for(THREAD_TEST_SHORT_DELAY);
        th.join(); // XXX must do this or it all crashes
      }

      // Skip auto-start
      {
        baton b;
        twine::thread th(thread_sleep, &b, false);
        CPPUNIT_ASSERT_EQUAL(false, th.joinable());
        twine::this_thread::sleep_for(THREAD_TEST_SHORT_DELAY);
        CPPUNIT_ASSERT_EQUAL(twine::thread::bad_thread_id, th.get_id());
        CPPUNIT_ASSERT(twine::this_thread::get_id() != th.get_id());
      }

      // Start and detach immediately
      {
        baton b;
        twine::thread th(thread_sleep, &b, true, true);
        CPPUNIT_ASSERT_EQUAL(false, th.joinable());
        twine::this_thread::sleep_for(THREAD_TEST_SHORT_DELAY);
        CPPUNIT_ASSERT_EQUAL(twine::thread::bad_thread_id, th.get_id());
        CPPUNIT_ASSERT(twine::this_thread::get_id() != th.get_id());
      }

      // Create without func
      {
        baton b;
        twine::thread th;
        CPPUNIT_ASSERT_EQUAL(false, th.joinable());
        twine::this_thread::sleep_for(THREAD_TEST_SHORT_DELAY);
        CPPUNIT_ASSERT_EQUAL(twine::thread::bad_thread_id, th.get_id());
        CPPUNIT_ASSERT(twine::this_thread::get_id() != th.get_id());
      }

      // Create, add func later
      {
        baton b;
        twine::thread th;
        CPPUNIT_ASSERT_EQUAL(false, th.joinable());
        twine::this_thread::sleep_for(THREAD_TEST_SHORT_DELAY);
        CPPUNIT_ASSERT_EQUAL(twine::thread::bad_thread_id, th.get_id());
        CPPUNIT_ASSERT(twine::this_thread::get_id() != th.get_id());

        th.set_func(thread_sleep, &b);
        CPPUNIT_ASSERT_EQUAL(false, th.joinable());
        twine::this_thread::sleep_for(THREAD_TEST_SHORT_DELAY);
        CPPUNIT_ASSERT_EQUAL(twine::thread::bad_thread_id, th.get_id());
        CPPUNIT_ASSERT(twine::this_thread::get_id() != th.get_id());
      }

      // Create, start later
      {
        baton b;
        twine::thread th;
        CPPUNIT_ASSERT_EQUAL(false, th.joinable());
        twine::this_thread::sleep_for(THREAD_TEST_SHORT_DELAY);
        CPPUNIT_ASSERT_EQUAL(twine::thread::bad_thread_id, th.get_id());
        CPPUNIT_ASSERT(twine::this_thread::get_id() != th.get_id());

        th.set_func(thread_sleep, &b);
        CPPUNIT_ASSERT_EQUAL(false, th.joinable());
        twine::this_thread::sleep_for(THREAD_TEST_SHORT_DELAY);
        CPPUNIT_ASSERT_EQUAL(twine::thread::bad_thread_id, th.get_id());
        CPPUNIT_ASSERT(twine::this_thread::get_id() != th.get_id());

        th.start();
        CPPUNIT_ASSERT_EQUAL(true, th.joinable());
        twine::this_thread::sleep_for(THREAD_TEST_SHORT_DELAY);
        CPPUNIT_ASSERT(twine::thread::bad_thread_id != th.get_id());
        CPPUNIT_ASSERT(twine::this_thread::get_id() != th.get_id());

        twine::this_thread::sleep_for(THREAD_TEST_SHORT_DELAY);
        th.join(); // XXX must do this or it all crashes
      }

      // Create, start & detach later
      {
        baton b;
        twine::thread th;
        CPPUNIT_ASSERT_EQUAL(false, th.joinable());
        CPPUNIT_ASSERT_EQUAL(false, th.joinable());
        twine::this_thread::sleep_for(THREAD_TEST_SHORT_DELAY);
        CPPUNIT_ASSERT_EQUAL(twine::thread::bad_thread_id, th.get_id());
        CPPUNIT_ASSERT(twine::this_thread::get_id() != th.get_id());

        th.set_func(thread_sleep, &b);
        CPPUNIT_ASSERT_EQUAL(false, th.joinable());
        CPPUNIT_ASSERT_EQUAL(false, th.joinable());
        twine::this_thread::sleep_for(THREAD_TEST_SHORT_DELAY);
        CPPUNIT_ASSERT_EQUAL(twine::thread::bad_thread_id, th.get_id());
        CPPUNIT_ASSERT(twine::this_thread::get_id() != th.get_id());

        th.start(true);
        CPPUNIT_ASSERT_EQUAL(false, th.joinable());
        CPPUNIT_ASSERT_EQUAL(false, th.joinable());
        twine::this_thread::sleep_for(THREAD_TEST_SHORT_DELAY);
        CPPUNIT_ASSERT_EQUAL(twine::thread::bad_thread_id, th.get_id());
        CPPUNIT_ASSERT(twine::this_thread::get_id() != th.get_id());
      }
    }



    void testMultipleThreads()
    {
      baton b;

      // First test that the baton actually gets it's counter incremented.
      {
        CPPUNIT_ASSERT_EQUAL(int(0), b.count);

        twine::thread th0(thread_incr, &b);

        twine::this_thread::sleep_for(THREAD_TEST_SHORT_DELAY);

        th0.join();

        // Should be incremented by one
        CPPUNIT_ASSERT_EQUAL(int(1), b.count);
      }

      // Next, test decrementing the counter
      {
        CPPUNIT_ASSERT_EQUAL(int(1), b.count);

        twine::thread th0(thread_decr, &b);

        twine::this_thread::sleep_for(THREAD_TEST_SHORT_DELAY);

        th0.join();

        // Should be decremented by one
        CPPUNIT_ASSERT_EQUAL(int(0), b.count);
      }

      // Last, do both concurrently
      {
        CPPUNIT_ASSERT_EQUAL(int(0), b.count);

        twine::thread th0(thread_incr, &b);
        twine::thread th1(thread_decr, &b);

        twine::this_thread::sleep_for(THREAD_TEST_SHORT_DELAY);

        th0.join();
        th1.join();

        // After both threads have been successfully joined, the baton's counter
        // should still be zero - incremented once and decremented once
        CPPUNIT_ASSERT_EQUAL(int(0), b.count);
      }
    }


    void testBinder()
    {
      // Creation auto-starts
      {
        bind_test test;
        CPPUNIT_ASSERT_EQUAL(false, test.called);

        twine::thread th(
            twine::thread::binder<bind_test, &bind_test::member>::function,
            &test);
        CPPUNIT_ASSERT_EQUAL(true, th.joinable());
        twine::this_thread::sleep_for(THREAD_TEST_SHORT_DELAY);
        CPPUNIT_ASSERT(twine::thread::bad_thread_id != th.get_id());
        CPPUNIT_ASSERT(twine::this_thread::get_id() != th.get_id());

        twine::this_thread::sleep_for(THREAD_TEST_SHORT_DELAY);
        th.join(); // XXX must do this or it all crashes

        CPPUNIT_ASSERT_EQUAL(true, test.called);
      }
    }



    void testHardwareConcurrency()
    {
      // Just check the function returns more than zero - that means on the
      // platform the tests run, some sort of concurrency can be determined.
      CPPUNIT_ASSERT(twine::thread::hardware_concurrency() > 0);
    }
};


CPPUNIT_TEST_SUITE_REGISTRATION(ThreadTest);
