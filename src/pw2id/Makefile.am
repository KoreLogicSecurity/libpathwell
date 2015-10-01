
bin_PROGRAMS = pathwell-pw2id

EXTRA_DIST = all-includes.h app-includes.h pw2id.h sys-includes.h

pathwell_pw2id_SOURCES = pw2id.c

pathwell_pw2id_LDFLAGS = -L$(top_builddir)/src/libpathwell/.libs -lpathwell

pathwell_pw2id_CFLAGS = @pcre_cflags@ @sqlite_cflags@

include $(top_srcdir)/utils/common.am
