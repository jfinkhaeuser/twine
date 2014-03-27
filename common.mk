# Always include top-level source directory
AM_CXXFLAGS = -I$(top_srcdir) @AM_CXXFLAGS@

# Set default compiler flags
AM_CXXFLAGS += -g $(TWINE_CXX0X_FLAG)

# Dependencies XXX keep these in sync with twine.pc.in
AM_CXXFLAGS += $(PTHREAD_CFLAGS) $(META_CFLAGS)
# AM_LDFLAGS += 

# Build object files in subdirectories so we can support platform-dependent
# builds
AUTOMAKE_OPTIONS = subdir-objects
