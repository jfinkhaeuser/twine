twine
=====

Twine is a very small threading library for C++.

It's similar in scope to [TinyThread++](http://tinythreadpp.bitsnbites.eu/), so
let's blatantly steal from that project's description:

> Need portable threads for your C++ app? Is C++11 unavailable for your target
> compiler(s)? Is Boost too large?

So why start twine if `TinyThread++` already meets those requirements?

- Twine extends what the C++ standard requires to address a number of
  shortcomings in the standard (and also `TinyThread++`).
- Twine doesn't require embedding into your project, but can be installed
  system-wide; in particular it supports the
  [pkg-config](http://www.freedesktop.org/wiki/Software/pkg-config/) standard.
- Twine adds a few convenient multithreading-related constructs such as the
  `tasklet` class, extended functionality, etc.

That said, `TinyThread++` may well be what you're looking for, precisely because
those differences have side-effects you might not like. It's cool, I won't hate
you.


Features
--------

- Dual-licensed! Yes, that's a feature - it means you can use it in FLOSS for
  free, but can get a license for commercial software, too.
- Portable - tested under Mac OS X, Linux, FreeBSD and should work under most
  POSIX compatible systems.
- Still relatively faithful to the C++11 standard. That doesn't mean the entire
  standard's thread-related scope is implemented.
- Minimal overhead - most functions generate compact inline code.


Requirements
------------

- [CMake](http://www.cmake.org/) for the build system.
- Twine uses [CppUnit](http://cppunit.sourceforge.net) _for unit tests only_.
- Twine uses a very limited subset of [meta](https://github.com/unwesen/meta).
  It's so limited it's barely necessary, but hey, `pkg-config` makes it easy!


Installation
------------

After installing the requirements, run:

```bash
$ cmake .
```

This will configure the build system. You might want to make sure dependencies
are found if they're not installed system wide:

```bash
$ PKG_CONFIG_PATH=/path/to/dir/lib/pkgconfig cmake .
```

Additionally, you might want to specify an installation prefix for the library
and header files:

```bash
$ cmake -DCMAKE_INSTALL_PREFIX=/path/to/dir .
```

You can also switch on C++11 mode, which primarily uses the meta library with
all C++11 features enabled.

```bash
$cmake -DTWINE_USE_CXX11=1 .
```

Usually, you can just run the following commands to get going:

```bash
$ make testsuite && ./testsuite
```

Install using the `DESTDIR` environment variable, if necessary:

```bash
$ make DESTDIR=/some/prefix install
```
