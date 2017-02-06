/**
 * This file is part of twine.
 *
 * Author(s): Jens Finkhaeuser <jens@finkhaeuser.de>
 *
 * Copyright (c) 2017 Jens Finkhaeuser.
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
#include <twine/chrono.h>

#include <meta/nullptr.h>

namespace twine {
namespace chrono {

TWINE_ANONS_START

/**
 * We define a reference point for clocks_gettime(), based off the start of the
 * program (static initialization).
 */
struct time_reference
{
  time_reference()
  {
    // Get the current system time as a reference.
    SYSTEMTIME  system_time;
    GetSystemTime(&system_time);

    FILETIME    file_time;
    SystemTimeToFileTime(&system_time, &file_time);

    m_reference =  ((uint64_t)file_time.dwLowDateTime);
    m_reference += ((uint64_t)file_time.dwHighDateTime) << 32;

    // Relative to EPOCH
    m_reference -= ((uint64_t) 116444736000000000ULL);

    // Grab starting time.
    QueryPerformanceCounter(&m_starting_time);

    // Also get QPC frequency
    QueryPerformanceFrequency(&m_frequency);
  }

  int64_t       m_reference;
  LARGE_INTEGER m_frequency;
  LARGE_INTEGER m_starting_time;
};

time_reference const global_reference = time_reference();

TWINE_ANONS_END


nanoseconds
now()
{
  // Based on https://msdn.microsoft.com/en-gb/library/windows/desktop/dn553408(v=vs.85).aspx
  LARGE_INTEGER qpc;
  QueryPerformanceCounter(&qpc);

  // Make a diff from the global reference's start; we have the elapsed time
  // in ticks.
  qpc.QuadPart -= global_reference.m_starting_time.QuadPart;

  // Now to convert ticks to nanoseconds. We multiply before dividing by the
  // frequency to guard against loss of precision.
  qpc.QuadPart *= 1000;
  qpc.QuadPart /= global_reference.m_frequency.QuadPart;

  // So now we have elapsed nanoseconds since the program started; we now need
  // to make this absolute from the program start.
  return nanoseconds(global_reference.m_reference + qpc.QuadPart);
}



bool
sleep(nanoseconds const & nsec)
{
  // FIXME
  return false;
}


}} // namespace twine::chrono
