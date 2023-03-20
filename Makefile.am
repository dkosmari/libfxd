ACLOCAL_AMFLAGS = -I m4

EXTRA_DIST = \
	bootstrap \
	COPYING.APL \
	README.md

SUBDIRS = \
	benchmarks \
	doc \
	examples \
	include/fxd \
	tests

.PHONY: doc bench

doc:
	cd doc && $(MAKE) $(AM_MAKEFLAGS) clean html
