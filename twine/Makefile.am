include $(top_srcdir)/common.mk

SUBDIRS = .

if HAVE_POSIX
SUBDIRS += posix
endif

if HAVE_WIN32
SUBDIRS += win32
endif

include_HEADERS = \
	twine.h \
	twine-config.h \
	version.h \
	mutex.h \
	chrono.h \
	thread.h \
	condition.h
