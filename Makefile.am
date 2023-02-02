ACLOCAL_AMFLAGS = -I m4

EXTRA_DIST = \
	bootstrap \
	COPYING.APL

SUBDIRS = \
	include/fxd \
	doc \
	examples \
	tests

.PHONY: doc
doc:
	cd doc && $(MAKE) $(AM_MAKEFLAGS) clean html
