
EXTRA_DIST = DESCRIPTION.pod EXAMPLES.pod NAME.pod OPTIONS.pod RETURN_VALUES.pod SEE_ALSO.pod SYNOPSIS.pod

man_MANS = pathwell-pw2id.1

pathwell-pw2id.pod:
	cat $(srcdir)/NAME.pod $(srcdir)/SYNOPSIS.pod $(srcdir)/DESCRIPTION.pod $(srcdir)/OPTIONS.pod $(srcdir)/RETURN_VALUES.pod $(srcdir)/EXAMPLES.pod $(srcdir)/SEE_ALSO.pod > $@
	
pathwell-pw2id.1: pathwell-pw2id.pod
	pod2man --release='$(PACKAGE_VERSION)' --center='$(PACKAGE_NAME)' --section=1 pathwell-pw2id.pod $@

CLEANFILES = pathwell-pw2id.pod pathwell-pw2id.1

include $(top_srcdir)/utils/common.am
