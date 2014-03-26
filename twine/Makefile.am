include $(top_srcdir)/common.mk

SUBDIRS = .

pkginclude_HEADERS = \
	twine.h \
	twine-config.h \
	version.h \
	mutex.h \
	scoped_lock.h \
	chrono.h \
	thread.h \
	condition.h

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
