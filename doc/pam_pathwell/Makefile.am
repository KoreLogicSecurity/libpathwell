
EXTRA_DIST = DESCRIPTION.pod EXAMPLES.pod NAME.pod OPTIONS.pod RETURN_VALUES.pod SEE_ALSO.pod SYNOPSIS.pod

man_MANS = pam_pathwell.8

pam_pathwell.pod:
	cat $(srcdir)/NAME.pod $(srcdir)/SYNOPSIS.pod $(srcdir)/DESCRIPTION.pod $(srcdir)/OPTIONS.pod $(srcdir)/RETURN_VALUES.pod $(srcdir)/EXAMPLES.pod $(srcdir)/SEE_ALSO.pod > $@
	
pam_pathwell.8: pam_pathwell.pod
	pod2man --release='$(PACKAGE_VERSION)' --center='$(PACKAGE_NAME)' --section=8 pam_pathwell.pod $@

CLEANFILES = pam_pathwell.pod pam_pathwell.8

include $(top_srcdir)/utils/common.am
