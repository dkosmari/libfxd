# examples/Makefile.am

noinst_HEADERS = printer.hpp

DEFAULT_INCLUDES =

AM_CPPFLAGS = \
	-I$(top_srcdir)/include


AM_CXXFLAGS = \
	-Wall -Wextra


AM_DEFAULT_SOURCE_EXT = .cpp

noinst_PROGRAMS = \
	basic \
	closest-pair \
	matrix-mul \
	numbers \
	saturated \
	square-root


matrix_mul_CXXFLAGS = $(AM_CXXFLAGS) -frounding-math
matrix_square_root_CXXFLAGS = $(AM_CXXFLAGS) -frounding-math


.PHONY: company

company: compile_flags.txt

compile_flags.txt: Makefile
	echo "$(AM_CPPFLAGS) $(CPPFLAGS) $(DEFS)" | xargs -n1 | sort -u > $(srcdir)/compile_flags.txt
