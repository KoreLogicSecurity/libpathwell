
EXTRA_DIST = all-includes.h app-includes.h pam_pathwell.h sys-includes.h

securelibdir = $(SECUREDIR)

securelib_LTLIBRARIES = pam_pathwell.la

pam_pathwell_la_SOURCES = pam_pathwell.c

pam_pathwell_la_LDFLAGS = -module -version-info @pam_pathwell_version@ -L$(top_builddir)/src/libpathwell/.libs -lpathwell @pam_ldflags@ @pam_ldadd@

pam_pathwell_la_CFLAGS = @pam_cflags@

include $(top_srcdir)/utils/common.am
