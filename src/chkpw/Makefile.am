
bin_PROGRAMS = pathwell-chkpw

EXTRA_DIST = all-includes.h app-includes.h chkpw.h sys-includes.h

pathwell_chkpw_SOURCES = chkpw.c

pathwell_chkpw_LDFLAGS = -L$(top_builddir)/src/libpathwell/.libs -lpathwell

include $(top_srcdir)/utils/common.am
