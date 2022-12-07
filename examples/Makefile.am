# examples/Makefile.am

AM_CPPFLAGS = \
	-I$(top_srcdir)/include


AM_CXXFLAGS = \
	-Wall


AM_DEFAULT_SOURCE_EXT = .cpp

noinst_PROGRAMS = \
	basic \
	saturated


.PHONY: company

company: compile_flags.txt

compile_flags.txt: Makefile
	echo "$(AM_CPPFLAGS) $(CPPFLAGS) $(DEFS)" | xargs -n1 | sort -u > $(srcdir)/compile_flags.txt
