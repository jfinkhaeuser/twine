include $(top_srcdir)/common.mk

SUBDIRS = .

lib_LTLIBRARIES = libtwine.la

pkginclude_HEADERS = \
	twine.h \
	twine-config.h \
	version.h \
	noncopyable.h \
	mutex.h \
	scoped_lock.h \
	chrono.h \
	thread.h \
	condition.h \
	binder.h \
	tasklet.h

nobase_pkginclude_HEADERS = \
	detail/unwrap_handle.h

if HAVE_POSIX
nobase_pkginclude_HEADERS += \
	posix/mutex.h \
	posix/mutex_policy.h \
	posix/condition.h
endif

if HAVE_WIN32
nobase_pkginclude_HEADERS += \
	win32/mutex.h \
	win32/mutex_policy.h \
	win32/condition.h
endif

libtwine_la_CPPFLAGS = $(DEPS_CFLAGS)
libtwine_la_LIBADD = $(DEPS_LIBS)
libtwine_la_LDFLAGS = -version-info 1:0:0

libtwine_la_SOURCES = \
	version.cpp \
	chrono.cpp \
	tasklet.cpp

if HAVE_POSIX
libtwine_la_SOURCES += \
	posix/thread.cpp
endif

if HAVE_WIN32
libtwine_la_SOURCES += \
	win32/thread.cpp
endif
