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

#include <twine/binder.h>

namespace {


struct foo
{
  void bar(void * baton)
  {
    CPPUNIT_ASSERT_EQUAL(static_cast<void *>(this), baton);
  }

  void quux(int & a, void * baton)
  {
    CPPUNIT_ASSERT_EQUAL(static_cast<void *>(this), baton);
    CPPUNIT_ASSERT_EQUAL(int(42), a);
  }
};


} // anonymous namespace

class BinderTest
    : public CppUnit::TestFixture
{
public:
    CPPUNIT_TEST_SUITE(BinderTest);

      CPPUNIT_TEST(testNoParam);
      CPPUNIT_TEST(testOneParam);

    CPPUNIT_TEST_SUITE_END();

private:

    void testNoParam()
    {
      foo f;
      twine::binder0<foo, &foo::bar>::function(&f);
    }



    void testOneParam()
    {
      int a = 42;
      foo f;
      twine::binder1<int &, foo, &foo::quux>::function(a, &f);
    }

};


CPPUNIT_TEST_SUITE_REGISTRATION(BinderTest);
