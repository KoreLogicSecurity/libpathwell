
EXTRA_DIST = DESCRIPTION.pod EXAMPLES.pod NAME.pod OPTIONS.pod RETURN_VALUES.pod SEE_ALSO.pod SYNOPSIS.pod

man_MANS = pathwell-topology2id.1

pathwell-topology2id.pod:
	cat $(srcdir)/NAME.pod $(srcdir)/SYNOPSIS.pod $(srcdir)/DESCRIPTION.pod $(srcdir)/OPTIONS.pod $(srcdir)/RETURN_VALUES.pod $(srcdir)/EXAMPLES.pod $(srcdir)/SEE_ALSO.pod > $@
	
pathwell-topology2id.1: pathwell-topology2id.pod
	pod2man --release='$(PACKAGE_VERSION)' --center='$(PACKAGE_NAME)' --section=1 pathwell-topology2id.pod $@

CLEANFILES = pathwell-topology2id.pod pathwell-topology2id.1

include $(top_srcdir)/utils/common.am
