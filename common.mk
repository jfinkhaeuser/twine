# Always include top-level source directory
AM_CXXFLAGS = -I$(top_srcdir) @AM_CXXFLAGS@

# Set default compiler flags
AM_CXXFLAGS += -g --std=c++0x

# Dependencies XXX keep these in sync with twine.pc.in
AM_CXXFLAGS += $(BOOST_CPPFLAGS)
# AM_LDFLAGS += 
