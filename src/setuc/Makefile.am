
bin_PROGRAMS = pathwell-setuc

EXTRA_DIST = all-includes.h app-includes.h setuc.h sys-includes.h

pathwell_setuc_SOURCES = setuc.c

pathwell_setuc_LDFLAGS = -L$(top_builddir)/src/libpathwell/.libs -lpathwell

include $(top_srcdir)/utils/common.am
