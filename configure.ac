##############################################################################
# Setup
AC_PREREQ([2.68])
LT_PREREQ([2.4])

AC_INIT([twine], [0.1], [jens@unwesen.co.uk])
AC_CONFIG_SRCDIR([twine])
AC_CANONICAL_BUILD
AC_CANONICAL_HOST
AC_CANONICAL_TARGET
AM_INIT_AUTOMAKE

AC_CONFIG_HEADER([twine/_config.h])
AX_PREFIX_CONFIG_H([twine/twine-config.h])

LT_INIT
AC_SUBST([LIBTOOL_DEPS])
AC_CONFIG_MACRO_DIR([m4])

AC_LANG_CPLUSPLUS

##############################################################################
# Make build target available to automake
target=
case $target_os in
  *linux*|*bsd*)
    target=posix
    ;;
  *win32*)
    target=win32
    ;;
esac
AM_CONDITIONAL([HAVE_POSIX], [test "x$target" == xposix])
AM_CONDITIONAL([HAVE_WIN32], [test "x$target" == xwin32])

##############################################################################
# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AM_PROG_LIBTOOL
PKG_PROG_PKG_CONFIG

##############################################################################
# Checks for compiler characteristics
AC_C_BIGENDIAN

# The AU_ALIAS is needed due to a bug in AX_CXX_HEADER_STDCXX_0X, etc.
AX_CXX_COMPILE_STDCXX_0X
AU_ALIAS([AC_COMPILE_STDCXX_0X], [AX_CXX_COMPILE_STDCXX_0X])

##############################################################################
# Checks for libraries.
AX_PTHREAD()
PKG_CHECK_MODULES([CPPUNIT], [cppunit >= 1.12])
PKG_CHECK_MODULES([META], [meta >= 0.1])

# Check if the CXX mode was set on the configure command line
if echo "$CXXFLAGS" | $GREP -q 'DTWINE_CXX_MODE=TWINE_CXX_MODE_CXX0X' ; then
  jf_cxx_mode=0x
elif echo "$CXXFLAGS" | $GREP -q 'DTWINE_CXX_MODE=TWINE_CXX_MODE_CXX98' ; then
  jf_cxx_mode=98
else
  jf_cxx_mode=0x
fi
AM_CONDITIONAL([TWINE_CXX_MODE_CXX0X], [test "$jf_cxx_mode" = 0x])
AM_CONDITIONAL([TWINE_CXX_MODE_CXX98], [test "$jf_cxx_mode" = 98])

##############################################################################
# Checks for header files.
AC_HEADER_STDC

AX_CXX_HEADER_STDCXX_98
AX_CXX_HEADER_STDCXX_TR1
AX_CXX_HEADER_STDCXX_0X

AC_CHECK_HEADERS([sys/select.h sys/time.h sys/types.h time.h])


##############################################################################
# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_INT32_T
AC_TYPE_INT64_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T

##############################################################################
# Checks for library functions.
CXXFLAGS="$CXXFLAGS $PTHREAD_CFLAGS"
JF_CHECK_FUNC_IN([unistd.h sys/syscall.h sys/types.h], [gettid],
    [int foo = syscall(SYS_gettid);])
JF_CHECK_FUNC_IN([pthread_np.h], [pthread_getthreadid_np],
    [int foo = pthread_getthreadid_np();])
JF_CHECK_FUNC_IN([sched.h], [sched_yield], [sched_yield();])
JF_CHECK_FUNC_IN([time.h], [nanosleep], [nanosleep(NULL, NULL);])
JF_CHECK_FUNC_IN([unistd.h], [_SC_NPROCESSORS_ONLN],
    [sysconf(_SC_NPROCESSORS_ONLN);])
JF_CHECK_FUNC_IN([unistd.h], [_SC_NPROC_ONLN],
    [sysconf(_SC_NPROC_ONLN);])


##############################################################################
# Optional programs

##############################################################################
# Variable substitions
JF_SPLIT_VERSION

AC_SUBST([AM_CXXFLAGS])
AC_SUBST([AM_LDFLAGS])

##############################################################################
# Output
AC_CONFIG_FILES([
  Makefile
  twine/Makefile
  test/Makefile
  twine.pc
])
AC_OUTPUT
