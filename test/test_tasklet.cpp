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

#include <cstdlib>

#include <twine/tasklet.h>

#define THREAD_TEST_SHORT_DELAY twine::chrono::milliseconds(1)
#define THREAD_TEST_LONG_DELAY  twine::chrono::milliseconds(100)

namespace {

static bool done = false;

void sleeper(twine::tasklet & t, void *)
{
  // Sleep until woken up.
  while (t.sleep()) {
    // tum-tee-tum.
  }
  done = true;
}

void sleep_halfsec(twine::tasklet & t, void *)
{
  t.sleep(twine::chrono::milliseconds(500));
  done = true;
}

static int count = 0;

void counter(twine::tasklet & t, void *)
{
  while (t.sleep()) {
    ++count;
  }
}

struct bind_test
{
  bool finished;

  bind_test()
    : finished(false)
  {
  }

  void sleep_member(twine::tasklet & t, void *)
  {
    // Sleep until woken up.
    while (t.sleep()) {
      // tum-tee-tum.
    }
    finished = true;
  }
};


} // anonymous namespace

class TaskletTest
  : public CppUnit::TestFixture
{
public:
  CPPUNIT_TEST_SUITE(TaskletTest);

    CPPUNIT_TEST(testTaskletSleep);
    CPPUNIT_TEST(testTaskletMemFun);
    CPPUNIT_TEST(testTaskletScope);
    CPPUNIT_TEST(testSharedCondition);

  CPPUNIT_TEST_SUITE_END();
private:


  void testTaskletSleep()
  {
    // By testing the msec-based version of sleep(), both functions are
    // tested nicely. It might be prudent to test them separately at some
    // point though.

    // Check whether sleep() correctly handles 0 msecs (no sleeping)
    {
      done = false;
      twine::tasklet task(sleeper);

      twine::chrono::nanoseconds t1 = twine::chrono::now();
      CPPUNIT_ASSERT(task.start());
      twine::this_thread::sleep_for(THREAD_TEST_LONG_DELAY);
      CPPUNIT_ASSERT(task.stop());
      CPPUNIT_ASSERT(task.wait());
      twine::chrono::nanoseconds t2 = twine::chrono::now();

      // The time difference must always be lower than the 1000 msecs
      // (1000000 nsecs) we specified as the sleep time.
      CPPUNIT_ASSERT((t2.raw() - t1.raw()) < 1000000000);
    }

    // Same test, but with a half-second sleep. Now the elapsed time must
    // be larger than half a second.
    {
      done = false;
      twine::tasklet task(sleep_halfsec);

      twine::chrono::nanoseconds t1 = twine::chrono::now();
      CPPUNIT_ASSERT(task.start());
      CPPUNIT_ASSERT(task.wait());
      twine::chrono::nanoseconds t2 = twine::chrono::now();

      // The time difference must be very close to the sleep time of
      // 500msec. We'll want no less than 0.1 msec difference.
      CPPUNIT_ASSERT(std::labs((t2.raw() - t1.raw()) - 500000000) < 1000000);
    }

    // Count how often the thread got woken. Since it sleeps indefinitely,
    // it should get woken exactly twice: once due to wakeup(), and once
    // due to stop(). The stop() one would not result in wake_count to be
    // incremented, though.
    {
      count = 0;
      twine::tasklet task(counter);

      CPPUNIT_ASSERT(task.start());

      // Wait until the thread is running. Otherwise, wakeup() won't be
      // able to do anything (unless the thread runs quickly).
      twine::this_thread::sleep_for(THREAD_TEST_LONG_DELAY);

      task.wakeup();

      // Wait until the wakup is handled. There's a possibility for a race
      // in that it's possible the thread has gone to sleep once more by
      // the time we're trying to wait on the condition, in which case the
      // thread will not notify us and stay in its sleep state.
      twine::this_thread::sleep_for(THREAD_TEST_LONG_DELAY);

      CPPUNIT_ASSERT_EQUAL(int(1), count);

      CPPUNIT_ASSERT(task.stop());
      CPPUNIT_ASSERT(task.wait());
    }
  }



  void testTaskletMemFun()
  {
    // Binding member functions is done pretty much manually in twine.
    bind_test test;
    twine::tasklet task(
        twine::tasklet::binder<bind_test, &bind_test::sleep_member>::function,
        &test);

    twine::chrono::nanoseconds t1 = twine::chrono::now();
    CPPUNIT_ASSERT(task.start());
    twine::this_thread::sleep_for(THREAD_TEST_LONG_DELAY);
    CPPUNIT_ASSERT(task.stop());
    CPPUNIT_ASSERT(task.wait());
    twine::chrono::nanoseconds t2 = twine::chrono::now();

    // The time difference must always be lower than the 1000 msecs
    // (1000000 nsecs) we specified as the sleep time.
    CPPUNIT_ASSERT((t2.raw() - t1.raw()) < 1000000000);
  }



  void testTaskletScope()
  {
    // Checks to determine whether tasklets that are destroyed before being
    // started, stopped or waited upon cause ugliness. These tests basically
    // only have to not segfault...

    // Unused task
    {
      twine::tasklet task(sleeper);
    }

    // started task
    {
      twine::tasklet task(sleeper);
      task.start();
    }

    // started & stopped task
    {
      twine::tasklet task(sleeper);
      task.start();
      task.stop();
    }
  }


  void testSharedCondition()
  {
    count = 0;

    twine::condition cond;
    twine::recursive_mutex mutex;

    twine::tasklet t1(&cond, &mutex, counter, nullptr, true);
    twine::tasklet t2(&cond, &mutex, counter, nullptr, true);

    twine::this_thread::sleep_for(THREAD_TEST_LONG_DELAY);

    t1.wakeup(); // also wakes up t2

    twine::this_thread::sleep_for(THREAD_TEST_LONG_DELAY);

    // Both get woken
    CPPUNIT_ASSERT_EQUAL(int(2), count);

    // One gets stopped by this
    t1.stop();
    twine::this_thread::sleep_for(THREAD_TEST_LONG_DELAY);
    CPPUNIT_ASSERT_EQUAL(int(3), count);

    // Now both should be stopped.
    t2.stop();
  }
};


CPPUNIT_TEST_SUITE_REGISTRATION(TaskletTest);
