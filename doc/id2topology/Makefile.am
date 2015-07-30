
EXTRA_DIST = DESCRIPTION.pod EXAMPLES.pod NAME.pod OPTIONS.pod RETURN_VALUES.pod SEE_ALSO.pod SYNOPSIS.pod

man_MANS = pathwell-id2topology.1

pathwell-id2topology.pod:
	cat $(srcdir)/NAME.pod $(srcdir)/SYNOPSIS.pod $(srcdir)/DESCRIPTION.pod $(srcdir)/OPTIONS.pod $(srcdir)/RETURN_VALUES.pod $(srcdir)/EXAMPLES.pod $(srcdir)/SEE_ALSO.pod > $@
	
pathwell-id2topology.1: pathwell-id2topology.pod
	pod2man --release='$(PACKAGE_VERSION)' --center='$(PACKAGE_NAME)' --section=1 pathwell-id2topology.pod $@

CLEANFILES = pathwell-id2topology.pod pathwell-id2topology.1

include $(top_srcdir)/utils/common.am
