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
#include <twine/thread.h>

#define COND_TEST_SHORT_DELAY twine::chrono::milliseconds(1)
#define COND_TEST_LONG_DELAY  twine::chrono::milliseconds(100)

namespace {

template <typename mutexT>
struct baton
{
  int               woken;
  bool              timed_out;
  mutexT            m;
  twine::condition  cond;

  baton()
    : woken(0)
    , timed_out(false)
  {
  }
};


template <typename mutexT>
void thread_func(void * arg)
{
  baton<mutexT> * b = static_cast<baton<mutexT> *>(arg);
  twine::scoped_lock<mutexT> lock(b->m);
  b->cond.wait(lock);
  ++b->woken;
}


template <typename mutexT>
void thread_func_wait(void * arg)
{
  baton<mutexT> * b = static_cast<baton<mutexT> *>(arg);
  twine::scoped_lock<mutexT> lock(b->m);
  b->timed_out = ! b->cond.timed_wait(lock, COND_TEST_SHORT_DELAY);
  ++b->woken;
}


} // anonymous namespace


class ConditionTest
    : public CppUnit::TestFixture
{
public:
    CPPUNIT_TEST_SUITE(ConditionTest);

      CPPUNIT_TEST(testConditionMutex);
      CPPUNIT_TEST(testConditionRecursiveMutex);

    CPPUNIT_TEST_SUITE_END();

private:

  template <typename mutexT>
  void testConditionImpl()
  {
    twine::condition cond;

    // Since we don't have several threads, we expect timed_wait() to time out
    {
      mutexT m;
      m.lock();
      bool ret = cond.timed_wait(m, COND_TEST_LONG_DELAY);
      CPPUNIT_ASSERT_EQUAL(false, ret);
    }

    // The same with a lock
    {
      mutexT m;
      twine::scoped_lock<mutexT> l(m);
      bool ret = cond.timed_wait(l, COND_TEST_LONG_DELAY);
      CPPUNIT_ASSERT_EQUAL(false, ret);
    }

    // Now use a condition to wake up a single thread
    {
      baton<mutexT> b;
      twine::thread th0(thread_func<mutexT>, &b);
      CPPUNIT_ASSERT_EQUAL(true, th0.joinable());

      twine::scoped_lock<mutexT> l(b.m);
      CPPUNIT_ASSERT_EQUAL(0, b.woken);
      l.unlock();

      // Give threads a chance to wake up before checking the result
      twine::this_thread::sleep_for(COND_TEST_SHORT_DELAY);
      b.cond.notify_one();
      twine::this_thread::sleep_for(COND_TEST_SHORT_DELAY);

      l.lock();
      CPPUNIT_ASSERT_EQUAL(1, b.woken);
      CPPUNIT_ASSERT_EQUAL(false, th0.joinable());
    }

    // Use a condition with two threads, see which one wakes up
    {
      baton<mutexT> b;
      twine::thread th0(thread_func<mutexT>, &b);
      twine::thread th1(thread_func<mutexT>, &b);
      CPPUNIT_ASSERT_EQUAL(true, th0.joinable());
      CPPUNIT_ASSERT_EQUAL(true, th1.joinable());

      twine::scoped_lock<mutexT> l(b.m);
      CPPUNIT_ASSERT_EQUAL(0, b.woken);
      l.unlock();

      // Give threads a chance to wake up before checking the result
      twine::this_thread::sleep_for(COND_TEST_SHORT_DELAY);
      b.cond.notify_one();
      twine::this_thread::sleep_for(COND_TEST_SHORT_DELAY);

      l.lock();
      CPPUNIT_ASSERT_EQUAL(1, b.woken);
      CPPUNIT_ASSERT(false == th0.joinable() || false == th1.joinable());
      CPPUNIT_ASSERT(true  == th0.joinable() || true  == th1.joinable());

      // Notify again to get the other thread
      l.unlock();
      twine::this_thread::sleep_for(COND_TEST_SHORT_DELAY);
      b.cond.notify_one();
      twine::this_thread::sleep_for(COND_TEST_SHORT_DELAY);
      l.lock();

      CPPUNIT_ASSERT_EQUAL(false, th0.joinable());
      CPPUNIT_ASSERT_EQUAL(false, th1.joinable());
    }

    // Use a condition with two threads, notify both
    {
      baton<mutexT> b;
      twine::thread th0(thread_func<mutexT>, &b);
      twine::thread th1(thread_func<mutexT>, &b);
      CPPUNIT_ASSERT_EQUAL(true, th0.joinable());
      CPPUNIT_ASSERT_EQUAL(true, th1.joinable());

      twine::scoped_lock<mutexT> l(b.m);
      CPPUNIT_ASSERT_EQUAL(0, b.woken);
      l.unlock();

      // Give threads a chance to wake up before checking the result
      twine::this_thread::sleep_for(COND_TEST_SHORT_DELAY);
      b.cond.notify_all();
      twine::this_thread::sleep_for(COND_TEST_SHORT_DELAY);

      l.lock();
      CPPUNIT_ASSERT_EQUAL(2, b.woken);
      CPPUNIT_ASSERT_EQUAL(false, th0.joinable());
      CPPUNIT_ASSERT_EQUAL(false, th1.joinable());
    }

    // Single thread again, but a timed wait. Wait long enough!
    {
      baton<mutexT> b;
      twine::thread th0(thread_func_wait<mutexT>, &b);
      CPPUNIT_ASSERT_EQUAL(true, th0.joinable());

      twine::scoped_lock<mutexT> l(b.m);
      CPPUNIT_ASSERT_EQUAL(0, b.woken);
      l.unlock();

      // Give threads a chance to wake up before checking the result
      twine::this_thread::sleep_for(COND_TEST_SHORT_DELAY);
      b.cond.notify_one();
      twine::this_thread::sleep_for(COND_TEST_LONG_DELAY);

      l.lock();
      CPPUNIT_ASSERT_EQUAL(1, b.woken);
      CPPUNIT_ASSERT_EQUAL_MESSAGE("may occasionally time out",
          false, b.timed_out);
      CPPUNIT_ASSERT_EQUAL(false, th0.joinable());
    }

    // Single thread again, but a timed wait. Time out!
    {
      baton<mutexT> b;
      twine::thread th0(thread_func_wait<mutexT>, &b);
      CPPUNIT_ASSERT_EQUAL(true, th0.joinable());

      twine::scoped_lock<mutexT> l(b.m);
      CPPUNIT_ASSERT_EQUAL(0, b.woken);
      l.unlock();

      // Give threads a chance to wake up before checking the result
      twine::this_thread::sleep_for(COND_TEST_LONG_DELAY);

      l.lock();
      CPPUNIT_ASSERT_EQUAL(1, b.woken);
      CPPUNIT_ASSERT_EQUAL(true, b.timed_out);
      CPPUNIT_ASSERT_EQUAL(false, th0.joinable());
    }
  }



  void testConditionMutex()
  {
    testConditionImpl<twine::mutex>();
  }



  void testConditionRecursiveMutex()
  {
    testConditionImpl<twine::recursive_mutex>();
  }
};


CPPUNIT_TEST_SUITE_REGISTRATION(ConditionTest);
