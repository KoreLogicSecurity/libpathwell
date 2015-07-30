
EXTRA_DIST = DESCRIPTION.pod NAME.pod SEE_ALSO.pod SYNOPSIS.pod

man_MANS = pathwell_charsets.5

pathwell_charsets.pod:
	cat $(srcdir)/NAME.pod $(srcdir)/SYNOPSIS.pod $(srcdir)/DESCRIPTION.pod $(srcdir)/SEE_ALSO.pod > $@
	
pathwell_charsets.5: pathwell_charsets.pod
	pod2man --release='$(PACKAGE_VERSION)' --center='$(PACKAGE_NAME)' --section=1 pathwell_charsets.pod $@

CLEANFILES = pathwell_charsets.pod pathwell_charsets.5

include $(top_srcdir)/utils/common.am
