
bin_PROGRAMS = pathwell-id2topology

EXTRA_DIST = all-includes.h app-includes.h id2topology.h sys-includes.h

pathwell_id2topology_SOURCES = id2topology.c

pathwell_id2topology_LDFLAGS = -L$(top_builddir)/src/libpathwell/.libs -lpathwell

pathwell_id2topology_CFLAGS = @pcre_cflags@ @sqlite_cflags@

include $(top_srcdir)/utils/common.am
