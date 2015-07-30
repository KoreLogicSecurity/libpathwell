
EXTRA_DIST = DESCRIPTION.pod NAME.pod SYNOPSIS.pod

man_MANS = pathwell_api.3

pathwell_api.pod:
	cat $(srcdir)/NAME.pod $(srcdir)/SYNOPSIS.pod $(srcdir)/DESCRIPTION.pod > $@
	
pathwell_api.3: pathwell_api.pod
	pod2man --release='$(PACKAGE_VERSION)' --center='$(PACKAGE_NAME)' --section=1 pathwell_api.pod $@

CLEANFILES = pathwell_api.pod pathwell_api.3

include $(top_srcdir)/utils/common.am
