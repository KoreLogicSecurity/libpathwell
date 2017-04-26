
lib_LTLIBRARIES = libpathwell.la

EXTRA_DIST = all-includes.h api.h app-includes.h hint.h support.h sys-includes.h

libpathwell_la_SOURCES = api.c support.c hint.c

libpathwell_la_LDFLAGS = -version-info @libpathwell_version@ @gmp_ldflags@ @gmp_ldadd@ @json_ldflags@ @json_ldadd@ @pcre_ldflags@ @pcre_ldadd@ @sqlite_ldflags@ @sqlite_ldadd@

libpathwell_la_CFLAGS = @gmp_cflags@ @json_cflags@ @pcre_cflags@ @sqlite_cflags@

include $(top_srcdir)/utils/common.am
