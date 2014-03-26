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


namespace {


} // anonymous namespace

class ChronoTest
    : public CppUnit::TestFixture
{
public:
    CPPUNIT_TEST_SUITE(ChronoTest);

      CPPUNIT_TEST(testRaw);
      CPPUNIT_TEST(testConversion);

    CPPUNIT_TEST_SUITE_END();

private:

    template <typename T>
    void testRawImpl()
    {
      // Value of 0
      {
        T n(0);
        CPPUNIT_ASSERT_EQUAL(twine::chrono::usec_t(0), n.raw());
      }

      // Value of 1
      {
        T n(1);
        CPPUNIT_ASSERT_EQUAL(twine::chrono::usec_t(1), n.raw());
      }

      // Maximum value
      {
        T n(~twine::chrono::usec_t(0));
        CPPUNIT_ASSERT_EQUAL(~twine::chrono::usec_t(0), n.raw());
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
      twine::chrono::usec_t VALUE,
      twine::chrono::usec_t NANOSEC,
      twine::chrono::usec_t MICROSEC,
      twine::chrono::usec_t MILLISEC,
      twine::chrono::usec_t SEC,
      twine::chrono::usec_t MIN,
      twine::chrono::usec_t HOUR
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
};


CPPUNIT_TEST_SUITE_REGISTRATION(ChronoTest);
