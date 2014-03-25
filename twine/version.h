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
#ifndef TWINE_VERSION_H
#define TWINE_VERSION_H

#ifndef __cplusplus
#error You are trying to include a C++ only header file
#endif

#include <twine/twine.h>

#include <utility>
#include <string>

namespace twine {

/**
 * XXX Note to developers (and users): consider the following definitions to be
 *     frozen. That is, you may add new definitions, or modify their values,
 *     but may not modify the definitions themselves (i.e. types, parameters).
 *
 *     That way users of this library can always rely, especially, on the
 *     version() function's prototype, and perform compatibility checks at
 *     runtime.
 **/

/**
 * Return the library version as a pair of two components.
 *
 * Depending on whether this build is a release build or not, the component are
 * either the branch name and subversion revision (development builds), or the
 * major and minor version numbers (release builds).
 **/
inline std::pair<std::string, std::string> version()
{
  return std::make_pair<std::string, std::string>(PACKAGE_MAJOR, PACKAGE_MINOR);
}


/**
 * Return the library version as a string, with appropriate copyright notice.
 **/
inline char const * const copyright_string()
{
  static auto ret = PACKAGE_NAME " " PACKAGE_VERSION " " PACKAGE_URL "\n"
    "Copyright (c) 2014 Unwesen Ltd.\n"
    "Licensed under the the GPLv3 for personal, educational or non-profit use.\n"
    "Other licensing options available; please contact the copyright holder for\n"
    "information."
    ;
  return ret;
}


/**
 * Returns a short string with licensing information.
 **/
inline char const * const license_string()
{
  static auto ret =
    "This software is licensed under the terms of the GNU GPLv3 for personal,\n"
    "educational and non-profit use. For all other uses, alternative license\n"
    "options are available. Please contact the copyright holder for additional\n"
    "information, stating your intended usage.\n"
    "\n"
    "You can find the full text of the GPLv3 in the COPYING file in this code\n"
    "distribution.\n"
    "\n"
    "This software is distributed on an \"AS IS\" BASIS, WITHOUT ANY WARRANTY;\n"
    "without even the implied warranty of MERCHANTABILITY or FITNESS FOR A\n"
    "PARTICULAR PURPOSE."
    ;
  return ret;
}

} // namespace twine

#endif // guard
