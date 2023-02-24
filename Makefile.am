ACLOCAL_AMFLAGS = -I m4

EXTRA_DIST = \
	bootstrap \
	COPYING.APL

SUBDIRS = \
	benchmarks \
	doc \
	examples \
	include/fxd \
	tests

.PHONY: doc
doc:
	cd doc && $(MAKE) $(AM_MAKEFLAGS) clean html
