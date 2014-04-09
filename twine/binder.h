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
#ifndef TWINE_BINDER_H
#define TWINE_BINDER_H

#ifndef __cplusplus
#error You are trying to include a C++ only header file
#endif

#include <twine/twine.h>

namespace twine {

/**
 * In order to bind member functions to threads and tasklets, we use an
 * extremely simple binding mechanism that, in order to remain so simple,
 * assumes the baton passed to the thread is the object whose member function
 * is to be bound.
 *
 * For the use of tasklets, there exists a variant that accepts a first argument
 * type, too.
 *
 * Use it like so:
 *
 *   struct foo
 *   {
 *     void bar(void * baton)
 *     {
 *       assert(baton == this);
 *     }
 *
 *     void quux(tasklet & t, void * baton)
 *     {
 *       assert(baton == this);
 *     }
 *   };
 *
 *   thread th(binder0<foo, &foo::bar>::function);
 *   tasklet tk(binder1<tasklet &, foo, &foo::quux>::function);
 **/

template <
  typename argT0,
  typename classT,
  void (classT::*funcT)(argT0, void *)
>
struct binder1
{
  static void function(argT0 arg0, void * baton)
  {
    (static_cast<classT *>(baton)->*funcT)(arg0, baton);
  }
};

template <
  typename classT,
  void (classT::*funcT)(void *)
>
struct binder0
{
  static void function(void * baton)
  {
    (static_cast<classT *>(baton)->*funcT)(baton);
  }
};


} // namespace twine

#endif // guard
