# tests/catch2/Makefile.am

EXTRA_DIST = COPYING.BSL

noinst_LTLIBRARIES = libcatch2.la

BUILT_SOURCES = catch_amalgamated.cpp catch_amalgamated.hpp

libcatch2_la_SOURCES = $(BUILT_SOURCES)


CATCH2_URL = "https://github.com/catchorg/Catch2/releases/latest/download"

.PHONY: update

update:
	wget "$(CATCH2_URL)/catch_amalgamated.cpp" -O "$(srcdir)/catch_amalgamated.cpp"
	touch catch_amalgamated.cpp
	wget "$(CATCH2_URL)/catch_amalgamated.hpp" -O "$(srcdir)/catch_amalgamated.hpp"
	touch catch_amalgamated.hpp

