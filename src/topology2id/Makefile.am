
bin_PROGRAMS = pathwell-topology2id

EXTRA_DIST = all-includes.h app-includes.h topology2id.h sys-includes.h

pathwell_topology2id_SOURCES = topology2id.c

pathwell_topology2id_LDFLAGS = -L$(top_builddir)/src/libpathwell/.libs -lpathwell

pathwell_topology2id_CFLAGS = @pcre_cflags@ @sqlite_cflags@

include $(top_srcdir)/utils/common.am
