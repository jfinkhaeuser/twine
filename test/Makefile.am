include $(top_srcdir)/common.mk

TESTS = testsuite
check_PROGRAMS = $(TESTS)

testsuite_CPPFLAGS = $(CPPUNIT_CFLAGS)
testsuite_LDADD = \
	$(CPPUNIT_LIBS) \
	$(PTHREAD_LIBS)

testsuite_SOURCES = \
	test_mutex.cpp \
	test_lock.cpp \
	test_thread.cpp \
	testsuite.cpp
