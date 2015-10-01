
ACLOCAL_AMFLAGS = -I m4

AUTOMAKE_OPTIONS = foreign

EXTRA_DIST = README README.CREDITS README.INSTALL README.LICENSE README.PAM README.SELinux VERSION VERSION.LIBRARY VERSION.MODULE VERSION.SCHEMA utils/version2string utils/version_helper

SUBDIRS = data doc include src tests

dist-hook:
	@rm -f `find $(distdir) -type f -name ChangeLog -o -name TODO`

dist-sign: dist
	@gpg --default-key "PathWell Project (Code Signing Key)" --detach-sign $(distdir).tar.gz

include $(top_srcdir)/utils/common.am
