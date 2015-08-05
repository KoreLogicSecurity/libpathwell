
bin_PROGRAMS = pathwell-create-db

EXTRA_DIST = all-includes.h app-includes.h create-db.h sys-includes.h

pathwell_create_db_SOURCES = create-db.c

pathwell_create_db_LDFLAGS = -L$(top_builddir)/src/libpathwell/.libs -lpathwell

pathwell_create_db_CFLAGS = @sqlite_cflags@

include $(top_srcdir)/utils/common.am
