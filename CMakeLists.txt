##############################################################################
# Project metadata
cmake_minimum_required(VERSION 2.8)
project(twine)

set (TWINE_PACKAGE_MAJOR 0)
set (TWINE_PACKAGE_MINOR 1)
set (TWINE_PACKAGE_URL https://github.com/jfinkhaeuser/twine)

##############################################################################
# Options
option(TWINE_USE_CXX11
    "Forces meta to use C++11 features." ON)

if (TWINE_USE_CXX11)
  set (META_CXX_MODE META_CXX_MODE_CXX0X)
else (TWINE_USE_CXX11)
  set (META_CXX_MODE META_CXX_MODE_CXX98)
endif (TWINE_USE_CXX11)


##############################################################################
# Dependencies
find_package(Threads)

include(FindPkgConfig)
pkg_check_modules(META meta>=0.1)
pkg_check_modules(CPPUNIT cppunit>=1.12)


##############################################################################
# Headers
include(CheckIncludeFileCXX)
check_include_file_cxx(sys/time.h TWINE_HAVE_SYS_TIME_H)
check_include_file_cxx(sys/types.h TWINE_HAVE_SYS_TYPES_H)
check_include_file_cxx(sys/select.h TWINE_HAVE_SYS_SELECT_H)
check_include_file_cxx(time.h TWINE_HAVE_TIME_H)
check_include_file_cxx(unistd.h TWINE_HAVE_UNISTD_H)


##############################################################################
# Checks for typedefs, structures, and compiler characteristics.
include(CheckTypeSize)
check_type_size(int32_t INT32_T)
check_type_size(uint32_t UINT32_T)
check_type_size(int64_t INT64_T)
check_type_size(uint64_t UINT64_T)
check_type_size(size_t SIZE_T)
check_type_size(ssize_t SSIZE_T)


##############################################################################
# Functions
include(CheckFunctionExists)
check_function_exists(nanosleep TWINE_HAVE_NANOSLEEP)
check_function_exists(sched_yield TWINE_HAVE_SCHED_YIELD)

SET(CMAKE_REQUIRED_LIBRARIES "${CMAKE_THREAD_LIBS_INIT}")
check_function_exists(pthread_getthreadid_np TWINE_HAVE_PTHREAD_GETTHREADID_NP)


##############################################################################
# Symbols
include(CheckCXXSourceCompiles)
check_cxx_source_compiles("
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>

int main(int, char**)
{
  int foo = syscall(SYS_gettid);
}
" TWINE_HAVE_GETTID)

check_cxx_source_compiles("
#include <sys/types.h>
#include <sys/sysctl.h>

int main(int, char**)
{
  int foo = HW_NCPU;
}
" TWINE_HAVE_HW_NCPU)

check_cxx_source_compiles("
#include <unistd.h>

int main(int, char**)
{
  int foo = sysconf(_SC_NPROCESSORS_ONLN);
}
" TWINE_HAVE__SC_NPROCESSORS_ONLN)

check_cxx_source_compiles("
#include <unistd.h>

int main(int, char**)
{
  int foo = sysconf(_SC_NPROC_ONLN);
}
" TWINE_HAVE__SC_NPROC_ONLN)



##############################################################################
# Platform checks
include(TestBigEndian)
test_big_endian(TWINE_BIGENDIAN)


##############################################################################
# Write configuration
# get_cmake_property(_variableNames VARIABLES)
# foreach (_variableName ${_variableNames})
#     message(STATUS "${_variableName}=${${_variableName}}")
# endforeach()

configure_file(
    "${PROJECT_SOURCE_DIR}/twine/twine-config.h.in"
    "${PROJECT_BINARY_DIR}/twine/twine-config.h"
)

configure_file(
    "${PROJECT_SOURCE_DIR}/twine.pc.in"
    "${PROJECT_BINARY_DIR}/twine.pc"
    @ONLY
)


##############################################################################
# Compiler flags

# Set compiler specific flags
if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  message("Selecting compiler flags for clang++")

  set (CLANGXX_CXX_FLAGS "-Wall -Wextra -Wabi -Wold-style-cast"
      CACHE STRING "Default flags for clang++")
  if (TWINE_USE_CXX11)
    set (CLANGXX_CXX_FLAGS_STD "-std=c++0x -Wc++0x-compat")
  else (TWINE_USE_CXX11)
    set (CLANGXX_CXX_FLAGS_STD "-std=c++98")
  endif (TWINE_USE_CXX11)
  set (CMAKE_CXX_FLAGS "${CLANGXX_CXX_FLAGS} ${CLANGXX_CXX_FLAGS_STD}")
  set (CMAKE_CXX_FLAGS_DEBUG "${CLANGXX_CXX_FLAGS} ${CLANGXX_CXX_FLAGS_STD} -O0 -ggdb")
  set (CMAKE_CXX_FLAGS_RELEASE "${CLANGXX_CXX_FLAGS} ${CLANGXX_CXX_FLAGS_STD} -O3 -ggdb")


elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  message("Selecting compiler flags for GNU c++")

  set (GNUCXX_CXX_FLAGS "-Wall -Wextra -Wabi -Wold-style-cast -Wstrict-null-sentinel"
      CACHE STRING "Default flags for G++")
  if (TWINE_USE_CXX11)
    set (GNUCXX_CXX_FLAGS_STD "-std=c++0x -Wc++0x-compat")
  else (TWINE_USE_CXX11)
    set (GNUCXX_CXX_FLAGS_STD "-std=c++98")
  endif (TWINE_USE_CXX11)
  set (CMAKE_CXX_FLAGS "${GNUCXX_CXX_FLAGS} ${GNUCXX_CXX_FLAGS_STD}")
  set (CMAKE_CXX_FLAGS_DEBUG "${GNUCXX_CXX_FLAGS} ${GNUCXX_CXX_FLAGS_STD} -O0 -ggdb")
  set (CMAKE_CXX_FLAGS_RELEASE "${GNUCXX_CXX_FLAGS} ${GNUCXX_CXX_FLAGS_STD} -O3 -ggdb")
endif ()

# Add include paths
include_directories(
    "${PROJECT_SOURCE_DIR}"
    "${PROJECT_BINARY_DIR}"
)

# Add library flags
set (CMAKE_CXX_FLAGS          "${META_CFLAGS} ${CPPUNIT_CFLAGS} ${CMAKE_CXX_FLAGS}")
set (CMAKE_CXX_FLAGS_DEBUG    "${META_CFLAGS} ${CPPUNIT_CFLAGS} ${CMAKE_CXX_FLAGS_DEBUG}")
set (CMAKE_CXX_FLAGS_RELEASE  "${META_CFLAGS} ${CPPUNIT_CFLAGS} ${CMAKE_CXX_FLAGS_RELEASE}")


##############################################################################
# Build artefacts
set(LIB_SOURCES
    twine/version.cpp
    twine/chrono.cpp
    twine/tasklet.cpp)

if (UNIX)
  set(LIB_SOURCES ${LIB_SOURCES}
      twine/posix/thread.cpp)
endif (UNIX)

if (WIN32)
  set(LIB_SOURCES ${LIB_SOURCES}
      twine/win32/thread.cpp)
endif (WIN32)

add_library(twine_static STATIC ${LIB_SOURCES})
add_library(twine_shared SHARED ${LIB_SOURCES})
set_target_properties(twine_static twine_shared PROPERTIES OUTPUT_NAME twine)

set_target_properties(twine_shared PROPERTIES
    SOVERSION "${TWINE_PACKAGE_MAJOR}"
    VERSION "${TWINE_PACKAGE_MAJOR}.${TWINE_PACKAGE_MINOR}.0"
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/twine/twine-config.h
    twine/twine.h
    twine/version.h
    twine/noncopyable.h
    twine/mutex.h
    twine/scoped_lock.h
    twine/chrono.h
    twine/thread.h
    twine/condition.h
    twine/binder.h
    twine/tasklet.h
    DESTINATION include/twine)

install(FILES
    twine/detail/unwrap_handle.h
    DESTINATION include/twine/detail)

if (UNIX)
install(FILES
    twine/posix/mutex.h
    twine/posix/mutex_policy.h
    twine/posix/condition.h
    DESTINATION include/twine/posix)
endif (UNIX)

if (WIN32)
install(FILES
    twine/win32/mutex.h
    twine/win32/mutex_policy.h
    twine/win32/condition.h
    DESTINATION include/twine/posix)
endif (WIN32)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/twine.pc
    DESTINATION lib/pkgconfig)

install(TARGETS
    twine_static ARCHIVE
    DESTINATION lib)
install(TARGETS
    twine_shared LIBRARY
    DESTINATION lib)


##############################################################################
# Tests
if (CPPUNIT_FOUND)
  # Tests compatible with all C++ versions
  set(TEST_SOURCES
      test/test_mutex.cpp
      test/test_lock.cpp
      test/test_chrono.cpp
      test/test_thread.cpp
      test/test_condition.cpp
      test/test_binder.cpp
      test/test_tasklet.cpp
  )

  add_executable(testsuite
      ${TEST_SOURCES}
      test/testsuite.cpp)
  target_link_libraries(testsuite
      twine_static
      ${CMAKE_THREAD_LIBS_INIT}
      ${DEP_LIBRARIES}
      ${CPPUNIT_LDFLAGS})

  add_test(NAME "Testsuite" COMMAND testsuite)
endif (CPPUNIT_FOUND)
