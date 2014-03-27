include $(top_srcdir)/common.mk

TESTS = testsuite
check_PROGRAMS = $(TESTS)

testsuite_CPPFLAGS = $(CPPUNIT_CFLAGS)
testsuite_LDADD = \
	$(top_builddir)/twine/libtwine.la \
	$(CPPUNIT_LIBS) \
	$(PTHREAD_LIBS) \
	$(META_LIBS)

testsuite_SOURCES = \
	test_mutex.cpp \
	test_lock.cpp \
	test_chrono.cpp \
	test_thread.cpp \
	test_condition.cpp \
	testsuite.cpp
