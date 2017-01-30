/**
 * This file is part of twine.
 *
 * Author(s): Jens Finkhaeuser <jens@finkhaeuser.de>
 *
 * Copyright (c) 2014 Unwesen Ltd.
 * Copyright (c) 2015-2017 Jens Finkhaeuser.
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

#if defined(TWINE_HAVE_SYS_TIME_H)
#  include <sys/time.h>
#endif

#if defined(TWINE_HAVE_TIME_H)
#  include <time.h>
#endif

#include <iostream>

#include <meta/math.h>


namespace twine {
namespace chrono {

/**
 * Representation type used most commonly. Still, the templates could use others.
 **/
typedef int64_t default_repr_t;


/**
 * Detail contains a ratio class for implementing the duration class below.
 **/
namespace detail {

// Standard ratios
typedef ::meta::math::ratio<default_repr_t, 1, 1000000000>  nanosecond_ratio;  // Ratio for the unit nanoseconds.
typedef ::meta::math::ratio<default_repr_t, 1, 1000000>     microsecond_ratio; // Ratio for the unit microseconds.
typedef ::meta::math::ratio<default_repr_t, 1, 1000>        millisecond_ratio; // Ratio for the unit milliseconds.
typedef ::meta::math::ratio<default_repr_t, 1>              second_ratio;      // Ratio for the unit seconds.
typedef ::meta::math::ratio<default_repr_t, 60>             minute_ratio;      // Ratio for the unit minutes.
typedef ::meta::math::ratio<default_repr_t, 3600>           hour_ratio;        // Ratio for the unit hours.


/**
 * Implementation of a chrono::duration class similar to the C++11 standard,
 * with some extended functionality.
 **/
template <
  typename reprT,
  typename ratioT
>
struct duration;

template <
  typename reprT,
  typename ratioT
>
inline std::ostream &
operator<<(std::ostream & os, duration<reprT, ratioT> const & d);



template <
  typename reprT,
  typename ratioT = ::meta::math::ratio<reprT, reprT(1)>
>
struct duration
  : public ratioT
{
  typedef reprT   repr_t;
  typedef ratioT  ratio_t;

  explicit duration(reprT const & repr = reprT())
    : m_repr(repr)
  {
  }


  template <typename target_ratioT>
  duration(duration<reprT, target_ratioT> const & other)
    : m_repr(other.template as<ratioT>())
  {
  }


  template <typename target_ratioT>
  duration<reprT, ratioT> &
  operator=(duration<reprT, target_ratioT> const & other)
  {
    m_repr = other.template as<ratioT>();
    return *this;
  }


  inline reprT raw() const
  {
    return m_repr;
  }

  /***************************************************************************
   * Conversion to other ratios
   **/
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

  template <
    typename target_ratioT
  >
  inline target_ratioT convert() const
  {
    return target_ratioT(as<target_ratioT>());
  }

  /***************************************************************************
   * Conversion to system time/duration values
   **/
#if defined(TWINE_HAVE_SYS_TIME_H)
  inline void as(::timeval & val) const
  {
    typedef ::meta::math::divide_ratios<ratioT, second_ratio> second_ratio_t;
    val.tv_sec = time_t((m_repr * second_ratio_t::DIVIDEND) / second_ratio_t::DIVISOR);

    typedef typename ::meta::math::invert<second_ratio_t>::type inverse_t;
    default_repr_t remainder = m_repr - ((default_repr_t(val.tv_sec) * inverse_t::DIVIDEND) / inverse_t::DIVISOR);

    typedef ::meta::math::divide_ratios<ratioT, microsecond_ratio> microsecond_ratio_t;
    val.tv_usec = long((remainder * microsecond_ratio_t::DIVIDEND) / microsecond_ratio_t::DIVISOR);
  }
#endif

#if defined(TWINE_HAVE_TIME_H)
  inline void as(::timespec & spec) const
  {
    typedef ::meta::math::divide_ratios<ratioT, second_ratio> second_ratio_t;
    spec.tv_sec = time_t((m_repr * second_ratio_t::DIVIDEND) / second_ratio_t::DIVISOR);

    typedef typename ::meta::math::invert<second_ratio_t>::type inverse_t;
    default_repr_t remainder = m_repr - ((default_repr_t(spec.tv_sec) * inverse_t::DIVIDEND) / inverse_t::DIVISOR);

    typedef ::meta::math::divide_ratios<ratioT, nanosecond_ratio> nanosecond_ratio_t;
    spec.tv_nsec = long((remainder * nanosecond_ratio_t::DIVIDEND) / nanosecond_ratio_t::DIVISOR);
  }
#endif

  /***************************************************************************
   * Simple arithmetic operators
   **/
  template <typename target_ratioT>
  inline duration & operator+=(duration<reprT, target_ratioT> const & other)
  {
    m_repr += other.template as<ratioT>();
    return *this;
  }

  template <typename target_ratioT>
  inline duration & operator-=(duration<reprT, target_ratioT> const & other)
  {
    m_repr -= other.template as<ratioT>();
    return *this;
  }

  template <typename target_ratioT>
  inline duration operator+(duration<reprT, target_ratioT> const & other) const
  {
    duration<reprT, ratioT> tmp = *this;
    tmp += other;
    return tmp;
  }

  template <typename target_ratioT>
  inline duration operator-(duration<reprT, target_ratioT> const & other) const
  {
    duration<reprT, ratioT> tmp = *this;
    tmp -= other;
    return tmp;
  }

  /***************************************************************************
   * Comparison operators
   **/
  template <typename target_ratioT>
  inline bool operator>(duration<reprT, target_ratioT> const & other) const
  {
    return (m_repr > other.template as<ratioT>());
  }

  template <typename target_ratioT>
  inline bool operator<(duration<reprT, target_ratioT> const & other) const
  {
    return (m_repr < other.template as<ratioT>());
  }

  template <typename target_ratioT>
  inline bool operator>=(duration<reprT, target_ratioT> const & other) const
  {
    return (m_repr >= other.template as<ratioT>());
  }

  template <typename target_ratioT>
  inline bool operator<=(duration<reprT, target_ratioT> const & other) const
  {
    return (m_repr <= other.template as<ratioT>());
  }

  template <typename target_ratioT>
  inline bool operator==(duration<reprT, target_ratioT> const & other) const
  {
    return (m_repr == other.template as<ratioT>());
  }

  template <typename target_ratioT>
  inline bool operator!=(duration<reprT, target_ratioT> const & other) const
  {
    return (m_repr != other.template as<ratioT>());
  }

private:

  friend std::ostream & operator<< <reprT, ratioT>(std::ostream & os, duration<reprT, ratioT> const & d);

  reprT m_repr;
};



} // namespace detail

// Standard duration types.
typedef detail::duration<default_repr_t, detail::nanosecond_ratio>  nanoseconds;  // Duration with the unit nanoseconds.
typedef detail::duration<default_repr_t, detail::microsecond_ratio> microseconds; // Duration with the unit microseconds.
typedef detail::duration<default_repr_t, detail::millisecond_ratio> milliseconds; // Duration with the unit milliseconds.
typedef detail::duration<default_repr_t, detail::second_ratio>      seconds;      // Duration with the unit seconds.
typedef detail::duration<default_repr_t, detail::minute_ratio>      minutes;      // Duration with the unit minutes.
typedef detail::duration<default_repr_t, detail::hour_ratio>        hours;        // Duration with the unit hours.


namespace detail {


template <
  typename reprT,
  typename ratioT
>
inline std::ostream &
operator<<(std::ostream & os, duration<reprT, ratioT> const & d)
{
  default_repr_t tmp = d.template as<twine::chrono::nanoseconds>();

  static const struct
  {
    reprT limit;
    char  sep;
    int   pad;
  } conv[] = {
    { 3600000000000, ':', -1 },
    {   60000000000, ':', 1 },
    {    1000000000, '.', 1 },
    {             1, '\0', 8 },
  };

  for (size_t i = 0 ; i < sizeof(conv) / sizeof(conv[0]) ; ++i) {
    // Get the value for the current position
    reprT val = tmp / conv[i].limit;
    tmp = tmp % conv[i].limit;

    // Display value with padding.
    reprT p = 1;
    for (int j = 0 ; j < conv[i].pad ; ++j) {
      p *= 10;
    }
    for (int j = 0 ; j < conv[i].pad ; ++j) {
      if (val < p) {
        os << "0";
      }
      p /= 10;
    }

    // Display value and separator
    os << val;
    if (conv[i].sep > 0) {
      os << conv[i].sep;
    }
    else {
      break;
    }
  }

  return os;
}

} // namespace detail


/**
 * Get the current timestampt with maximum resolution. Note that it's quite
 * possible that the underlying system does not support this resolution, so the
 * return value will have the underlying system's resolution.
 **/
nanoseconds now();

/**
 * Sleep for the specified nanoseconds. Returns true if woken up after the
 * specified time, false on error. Note that the underlying system call may
 * not have the resolution you specify here.
 **/
bool sleep(nanoseconds const & nsec);

}} // namespace twine::chrono


#endif // guard
