ACLOCAL_AMFLAGS = -I m4

LIBTOOL_DEPS = @LIBTOOL_DEPS@
libtool: $(LIBTOOL_DEPS)
	$(SHELL) ./config.status libtool

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = twine.pc

DISTCLEANFILES = _configs.sed twine/twine-config.h

SUBDIRS = \
	twine \
	test
