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

#include <twine/chrono.h>

#include <sstream>


namespace {


} // anonymous namespace

class ChronoTest
    : public CppUnit::TestFixture
{
public:
    CPPUNIT_TEST_SUITE(ChronoTest);

      CPPUNIT_TEST(testRaw);
      CPPUNIT_TEST(testConversion);
      CPPUNIT_TEST(testStreaming);
      CPPUNIT_TEST(testNow);

    CPPUNIT_TEST_SUITE_END();

private:

    template <typename T>
    void testRawImpl()
    {
      // Value of 0
      {
        T n(0);
        CPPUNIT_ASSERT_EQUAL(twine::chrono::nsec_t(0), n.raw());
      }

      // Value of 1
      {
        T n(1);
        CPPUNIT_ASSERT_EQUAL(twine::chrono::nsec_t(1), n.raw());
      }

      // Maximum value
      {
        T n(~twine::chrono::nsec_t(0));
        CPPUNIT_ASSERT_EQUAL(~twine::chrono::nsec_t(0), n.raw());
      }
    }

    void testRaw()
    {
      testRawImpl<twine::chrono::nanoseconds>();
      testRawImpl<twine::chrono::microseconds>();
      testRawImpl<twine::chrono::milliseconds>();
      testRawImpl<twine::chrono::seconds>();
      testRawImpl<twine::chrono::minutes>();
      testRawImpl<twine::chrono::hours>();
    }



    template <
      typename testT,
      twine::chrono::nsec_t VALUE,
      twine::chrono::nsec_t NANOSEC,
      twine::chrono::nsec_t MICROSEC,
      twine::chrono::nsec_t MILLISEC,
      twine::chrono::nsec_t SEC,
      twine::chrono::nsec_t MIN,
      twine::chrono::nsec_t HOUR
    >
    void testConversionImpl()
    {
      namespace tc = twine::chrono;

      testT n(VALUE);

      CPPUNIT_ASSERT_EQUAL_MESSAGE(typeid(testT).name(), NANOSEC,  n.template as<tc::nanoseconds>());
      CPPUNIT_ASSERT_EQUAL_MESSAGE(typeid(testT).name(), MICROSEC, n.template as<tc::microseconds>());
      CPPUNIT_ASSERT_EQUAL_MESSAGE(typeid(testT).name(), MILLISEC, n.template as<tc::milliseconds>());
      CPPUNIT_ASSERT_EQUAL_MESSAGE(typeid(testT).name(), SEC,      n.template as<tc::seconds>());
      CPPUNIT_ASSERT_EQUAL_MESSAGE(typeid(testT).name(), MIN,      n.template as<tc::minutes>());
      CPPUNIT_ASSERT_EQUAL_MESSAGE(typeid(testT).name(), HOUR,     n.template as<tc::hours>());

      // The timespec value is always the same
      ::timespec t;
      n.as(t);
      CPPUNIT_ASSERT_EQUAL(time_t(3600), t.tv_sec);
      CPPUNIT_ASSERT_EQUAL(long(0), t.tv_nsec);
    }

    void testConversion()
    {
      namespace tc = twine::chrono;

      // Nanoseconds
      testConversionImpl<tc::nanoseconds,
        3600000000000,
        3600000000000,
           3600000000,
              3600000,
                 3600,
                   60,
                    1>();

      // Microseconds
      testConversionImpl<tc::microseconds,
           3600000000,
        3600000000000,
           3600000000,
              3600000,
                 3600,
                   60,
                    1>();

      // Milliseconds
      testConversionImpl<tc::milliseconds,
              3600000,
        3600000000000,
           3600000000,
              3600000,
                 3600,
                   60,
                    1>();

      // Seconds
      testConversionImpl<tc::seconds,
                 3600,
        3600000000000,
           3600000000,
              3600000,
                 3600,
                   60,
                    1>();

      // Minutes
      testConversionImpl<tc::minutes,
                   60,
        3600000000000,
           3600000000,
              3600000,
                 3600,
                   60,
                    1>();

      // Hours
      testConversionImpl<tc::hours,
                    1,
        3600000000000,
           3600000000,
              3600000,
                 3600,
                   60,
                    1>();

    }



    template <
      typename testT,
      twine::chrono::nsec_t VALUE
    >
    void testStreamingImpl(std::string const & expected)
    {
      namespace tc = twine::chrono;

      testT n(VALUE);

      std::stringstream s;
      s << n;
      CPPUNIT_ASSERT_EQUAL(expected, s.str());
    }

    void testStreaming()
    {
      namespace tc = twine::chrono;

      testStreamingImpl<tc::nanoseconds, 3600000000001>("1:00:00.000000001");
      testStreamingImpl<tc::nanoseconds, 3600000000000>("1:00:00.000000000");
      testStreamingImpl<tc::nanoseconds, 3599999999999>("0:59:59.999999999");
      testStreamingImpl<tc::microseconds, 3600000001>("1:00:00.000001000");
      testStreamingImpl<tc::microseconds, 3600000000>("1:00:00.000000000");
      testStreamingImpl<tc::microseconds, 3599999999>("0:59:59.999999000");
      testStreamingImpl<tc::milliseconds, 3600001>("1:00:00.001000000");
      testStreamingImpl<tc::milliseconds, 3600000>("1:00:00.000000000");
      testStreamingImpl<tc::milliseconds, 3599999>("0:59:59.999000000");
      testStreamingImpl<tc::seconds, 3601>("1:00:01.000000000");
      testStreamingImpl<tc::seconds, 3599>("0:59:59.000000000");
      testStreamingImpl<tc::minutes, 61>("1:01:00.000000000");
      testStreamingImpl<tc::minutes, 59>("0:59:00.000000000");
      testStreamingImpl<tc::hours, 25>("25:00:00.000000000");

      testStreamingImpl<tc::nanoseconds, 3612345678901>("1:00:12.345678901");
    }



    void testNow()
    {
      // Not much we can test for as it's a system call. The only thing is that
      // if it's nanoseconds, we need to know it's >0
      auto now = twine::chrono::now();
      CPPUNIT_ASSERT(now.raw() > 0);

      CPPUNIT_ASSERT_MESSAGE("may fail if you have incorrect system time",
          now.as<twine::chrono::seconds>() > 1396277686);
    }
};


CPPUNIT_TEST_SUITE_REGISTRATION(ChronoTest);
