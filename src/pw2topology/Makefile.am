
bin_PROGRAMS = pathwell-pw2topology

EXTRA_DIST = all-includes.h app-includes.h pw2topology.h sys-includes.h

pathwell_pw2topology_SOURCES = pw2topology.c

pathwell_pw2topology_LDFLAGS = -L$(top_builddir)/src/libpathwell/.libs -lpathwell

pathwell_pw2topology_CFLAGS = @pcre_cflags@

include $(top_srcdir)/utils/common.am
