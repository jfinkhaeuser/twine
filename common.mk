# Always include top-level source directory
AM_CXXFLAGS = -I$(top_srcdir) -I$(top_builddir) @AM_CXXFLAGS@

# Set default compiler flags
AM_CXXFLAGS += -g

if TWINE_CXX_MODE_CXX0X
AM_CXXFLAGS += -std=c++0x -Wc++0x-compat
else
if TWINE_CXX_MODE_CXX98
AM_CXXFLAGS += -std=c++98 -DMETA_CXX_MODE=META_CXX_MODE_CXX98
endif
endif


# Dependencies XXX keep these in sync with twine.pc.in
AM_CXXFLAGS += $(PTHREAD_CFLAGS) $(META_CFLAGS)
# AM_LDFLAGS += 

# Build object files in subdirectories so we can support platform-dependent
# builds
AUTOMAKE_OPTIONS = subdir-objects
