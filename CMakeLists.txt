##############################################################################
# Project metadata
cmake_minimum_required(VERSION 2.8)
project(twine)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/CMakeModules)

set (TWINE_PACKAGE_MAJOR 1)
set (TWINE_PACKAGE_MINOR 0)
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
# Compiler flags #1
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

  set (GNUCXX_CXX_FLAGS "-Wall -Wextra -Wold-style-cast -Wstrict-null-sentinel"
      CACHE STRING "Default flags for G++")
  if (TWINE_USE_CXX11)
    set (GNUCXX_CXX_FLAGS_STD "-std=c++0x -Wc++0x-compat")
  else (TWINE_USE_CXX11)
    set (GNUCXX_CXX_FLAGS_STD "-std=c++98")
  endif (TWINE_USE_CXX11)
  set (CMAKE_CXX_FLAGS "${GNUCXX_CXX_FLAGS} ${GNUCXX_CXX_FLAGS_STD}")
  set (CMAKE_CXX_FLAGS_DEBUG "${GNUCXX_CXX_FLAGS} ${GNUCXX_CXX_FLAGS_STD} -O0 -ggdb")
  set (CMAKE_CXX_FLAGS_RELEASE "${GNUCXX_CXX_FLAGS} ${GNUCXX_CXX_FLAGS_STD} -O3 -ggdb")

elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  message("Selecting compiler flags for MSVC")

  set (MSVC_CXX_FLAGS "/analyze /GR /sdl /W4"
      CACHE STRING "Default flags for MSVC")
  if (TWINE_USE_CXX11)
    set (MSVC_CXX_FLAGS_STD "/std:c++latest")
  else (TWINE_USE_CXX11)
    set (MSVC_CXX_FLAGS_STD "/std:c++98")
  endif (TWINE_USE_CXX11)
  set (CMAKE_CXX_FLAGS "${MSVC_CXX_FLAGS} ${MSVC_CXX_FLAGS_STD}")
  set (CMAKE_CXX_FLAGS_DEBUG "${MSVC_CXX_FLAGS} ${MSVC_CXX_FLAGS_STD} ")
  set (CMAKE_CXX_FLAGS_RELEASE "${MSVC_CXX_FLAGS} ${MSVC_CXX_FLAGS_STD} /GL /Ox")
endif ()

##############################################################################
# Headers
include(CheckIncludeFileCXX)
check_include_file_cxx(sys/time.h TWINE_HAVE_SYS_TIME_H)
check_include_file_cxx(sys/types.h TWINE_HAVE_SYS_TYPES_H)
check_include_file_cxx(sys/select.h TWINE_HAVE_SYS_SELECT_H)
check_include_file_cxx(time.h TWINE_HAVE_TIME_H)
check_include_file_cxx(unistd.h TWINE_HAVE_UNISTD_H)
check_include_file_cxx(sys/thr.h TWINE_HAVE_SYS_THR_H)


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
check_function_exists(clock_gettime TWINE_HAVE_CLOCK_GETTIME)
check_function_exists(gettimeofday TWINE_HAVE_GETTIMEOFDAY)
check_function_exists(GetSystemInfo TWINE_HAVE_GETSYSTEMINFO)
check_function_exists(SwitchToThread TWINE_HAVE_SWITCHTOTHREAD)

SET(CMAKE_REQUIRED_LIBRARIES "${CMAKE_THREAD_LIBS_INIT}")
check_function_exists(pthread_getthreadid_np TWINE_HAVE_PTHREAD_GETTHREADID_NP)
check_function_exists(pthread_threadid_np TWINE_HAVE_PTHREAD_THREADID_NP)
check_function_exists(thr_self TWINE_HAVE_THR_SELF)


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
# Dependencies
find_package(Threads)
find_package(meta 1.1)
find_package(cppunit 1.12.1)

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
# Compiler flags #2

# Add include paths
include_directories(
    "${PROJECT_SOURCE_DIR}"
    "${PROJECT_BINARY_DIR}"
    "${META_INCLUDEDIR}"
)

# Add library flags
set (CMAKE_CXX_FLAGS          "${META_CFLAGS} ${CPPUNIT_CFLAGS} ${CMAKE_CXX_FLAGS}")
set (CMAKE_CXX_FLAGS_DEBUG    "${META_CFLAGS} ${CPPUNIT_CFLAGS} ${CMAKE_CXX_FLAGS_DEBUG}")
set (CMAKE_CXX_FLAGS_RELEASE  "${META_CFLAGS} ${CPPUNIT_CFLAGS} ${CMAKE_CXX_FLAGS_RELEASE}")


##############################################################################
# Build artefacts
set(LIB_SOURCES
    twine/version.cpp
    twine/thread.cpp
    twine/tasklet.cpp
)

if (UNIX)
  set(LIB_SOURCES ${LIB_SOURCES}
      twine/posix/chrono.cpp
      twine/posix/thread.cpp)
endif (UNIX)

if (WIN32)
  set(LIB_SOURCES ${LIB_SOURCES}
      twine/win32/chrono.cpp
      twine/win32/thread.cpp)
endif (WIN32)

add_library(twine_static STATIC ${LIB_SOURCES})
add_library(twine_shared SHARED ${LIB_SOURCES})
set_target_properties(twine_static twine_shared PROPERTIES
    OUTPUT_NAME twine)
set_target_properties(twine_shared PROPERTIES
    SOVERSION "${TWINE_PACKAGE_MAJOR}"
    VERSION "${TWINE_PACKAGE_MAJOR}.${TWINE_PACKAGE_MINOR}.0"
)

if (UNIX)
    set(PLATFORM_IMPL_PATH posix)
elseif(WIN32)
    set(PLATFORM_IMPL_PATH win32)
else()
    error("Unsupported platform.")
endif()

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
    twine/detail/unwrap_internals.h
    DESTINATION include/twine/detail)

install(FILES
    twine/${PLATFORM_IMPL_PATH}/mutex.h
    twine/${PLATFORM_IMPL_PATH}/mutex_policy.h
    twine/${PLATFORM_IMPL_PATH}/condition.h
    DESTINATION include/twine/posix)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/twine.pc
    DESTINATION lib/pkgconfig)

install(TARGETS
    twine_static twine_shared
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    COMPONENT library)

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

  if (CPPUNIT_LIBRARY_DEBUG)
    target_link_libraries(testsuite
        twine_static
        ${CMAKE_THREAD_LIBS_INIT}
        ${DEP_LIBRARIES}
        debug
        ${CPPUNIT_LIBRARY_DEBUG})
  endif ()
  target_link_libraries(testsuite
      twine_static
      ${CMAKE_THREAD_LIBS_INIT}
      ${DEP_LIBRARIES}
      optimized
      ${CPPUNIT_LIBRARY_RELEASE})

  # Since some time after 2.8.9
  if ("${CMAKE_VERSION}" VERSION_GREATER "2.8.10")
    target_include_directories(testsuite PRIVATE ${CPPUNIT_INCLUDE_DIR})
  else()
    include_directories(testsuite PRIVATE ${CPPUNIT_INCLUDE_DIR})
  endif()

  set_target_properties(testsuite PROPERTIES LINK_FLAGS "${TESTSUITE_LINK_FLAGS}")

  add_test(NAME "Testsuite" COMMAND testsuite)
  set(PROJECT_TEST_NAME testsuite)

  # Code coverage
  if(CMAKE_COMPILER_IS_GNUCXX AND CMAKE_BUILD_TYPE STREQUAL "Coverage")
    include(CodeCoverage)
    setup_target_for_coverage(coverage ${PROJECT_TEST_NAME} coverage)
    target_link_libraries(${PROJECT_TEST_NAME} gcov)
  endif()

  # May not work for older CMake versions
  set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT testsuite)
endif (CPPUNIT_FOUND)

##############################################################################
# CPack Section
include(InstallRequiredSystemLibraries)

set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Twine is a very small threading library for C++.")
set(CPACK_PACKAGE_VENDOR "finkhaeuser consulting")
set(CPACK_PACKAGE_CONTACT "jens@finkhaeuser.de")
set(CPACK_PACKAGE_VERSION_MAJOR "${TWINE_PACKAGE_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${TWINE_PACKAGE_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "0")

set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
# set(CPACK_PACKAGE_INSTALL_DIRECTORY "CMake ${CMake_VERSION_MAJOR}.${CMake_VERSION_MINOR}")

if(WIN32 AND NOT UNIX)
  set(CPACK_GENERATOR "NSIS;NSIS64")
  # There is a bug in NSI that does not handle full unix paths properly. Make
  # sure there is at least one set of four (4) backlasshes.
  set(CPACK_PACKAGE_ICON "${PROJECT_SOURCE_DIR}\\\\data\\\\tool-icon.bmp")
  # set(CPACK_NSIS_INSTALLED_ICON_NAME "bin\\\\MyExecutable.exe")
  set(CPACK_NSIS_DISPLAY_NAME "${CPACK_PACKAGE_INSTALL_DIRECTORY} twine")
  set(CPACK_NSIS_HELP_LINK "http://jfinkhaeuser.github.io/twine/")
  set(CPACK_NSIS_URL_INFO_ABOUT "http://jfinkhaeuser.github.io/twine/")
  set(CPACK_NSIS_CONTACT "jens@finkhaeuser.de")
  set(CPACK_NSIS_MODIFY_PATH ON)
else(WIN32 AND NOT UNIX)
  set(CPACK_GENERATOR "STGZ;TGZ;TBZ2;DEB")
  # set(CPACK_STRIP_FILES "bin/MyExecutable")
  set(CPACK_SOURCE_STRIP_FILES "")

  # Debian
  # set(CPACK_DEBIAN_PACKAGE_DEPENDS "libc6 (>= 2.3.1-6), libgcc1 (>= 1:3.4.2-12)")
endif(WIN32 AND NOT UNIX)

# set(CPACK_PACKAGE_EXECUTABLES "MyExecutable" "My Executable")

include(CPack)
