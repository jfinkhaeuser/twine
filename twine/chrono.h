/**
 * This file is part of twine.
 *
 * Author(s): Jens Finkhaeuser <jens@unwesen.co.uk>
 *
 * Copyright (c) 2014 Unwesen Ltd.
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
#ifndef TWINE_CHRONO_H
#define TWINE_CHRONO_H

#ifndef __cplusplus
#error You are trying to include a C++ only header file
#endif

#include <twine/twine.h>

#include <meta/math.h>


namespace twine {
namespace chrono {

/**
 * Microsecond type.
 **/
typedef int64_t usec_t;


/**
 * Detail contains a ratio class for implementing the duration class below.
 **/
namespace detail {


/**
 * Minimal implementation of a chrono::duration class similar to the C++11
 * standard.
 **/
template <
  typename reprT,
  typename ratioT = ::meta::math::ratio<reprT, reprT(1)>
>
struct duration
  : public ratioT
{
  explicit duration(reprT const & repr = reprT())
    : m_repr(repr)
  {
  }

  inline reprT raw() const
  {
    return m_repr;
  }

  template <
    typename target_ratioT
  >
  inline reprT as() const
  {
    typedef ::meta::math::divide_ratios<ratioT, target_ratioT> final_ratio_t;
    // std::cout << "converting from " << ratioT::DIVIDEND << "/" << ratioT::DIVISOR
    //   << " to " << target_ratioT::DIVIDEND << "/" << target_ratioT::DIVISOR
    //   << " -> " << final_ratio_t::DIVIDEND << "/" << final_ratio_t::DIVISOR
    //   << std::endl;
    return (m_repr * final_ratio_t::DIVIDEND) / final_ratio_t::DIVISOR;
  }

private:

  reprT m_repr;
};

// Standard ratios
typedef ::meta::math::ratio<usec_t, 1, 1000000000>  nanosecond_ratio;  // Ratio for the unit nanoseconds.
typedef ::meta::math::ratio<usec_t, 1, 1000000>     microsecond_ratio; // Ratio for the unit microseconds.
typedef ::meta::math::ratio<usec_t, 1, 1000>        millisecond_ratio; // Ratio for the unit milliseconds.
typedef ::meta::math::ratio<usec_t, 1>              second_ratio;      // Ratio for the unit seconds.
typedef ::meta::math::ratio<usec_t, 60>             minute_ratio;      // Ratio for the unit minutes.
typedef ::meta::math::ratio<usec_t, 3600>           hour_ratio;        // Ratio for the unit hours.

} // namespace detail

// Standard duration types.
typedef detail::duration<usec_t, detail::nanosecond_ratio>  nanoseconds;  // Duration with the unit nanoseconds.
typedef detail::duration<usec_t, detail::microsecond_ratio> microseconds; // Duration with the unit microseconds.
typedef detail::duration<usec_t, detail::millisecond_ratio> milliseconds; // Duration with the unit milliseconds.
typedef detail::duration<usec_t, detail::second_ratio>      seconds;      // Duration with the unit seconds.
typedef detail::duration<usec_t, detail::minute_ratio>      minutes;      // Duration with the unit minutes.
typedef detail::duration<usec_t, detail::hour_ratio>        hours;        // Duration with the unit hours.

}} // namespace twine::chrono


#endif // guard
