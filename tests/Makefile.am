# tests/Makefile.am


noinst_HEADERS = \
	printer.hpp \
	rng.hpp \
	setup.hpp


SUBDIRS = catch2


AM_CPPFLAGS = \
	-I$(top_srcdir)/include


AM_CXXFLAGS = \
	-Wall -Wextra -Werror \
	-frounding-math \
	-fsanitize=undefined \
	-fsanitize-undefined-trap-on-error

check_PROGRAMS = \
	test-add \
	test-conv \
	test-div \
	test-mul \
	test-sub


AM_DEFAULT_SOURCE_EXT = .cpp


LDADD = \
	catch2/libcatch2.a


TESTS = $(check_PROGRAMS)


.PHONY: company

company: compile_flags.txt

compile_flags.txt: Makefile
	echo "$(AM_CPPFLAGS) $(CPPFLAGS) $(DEFS)" | xargs -n1 | sort -u > $(srcdir)/compile_flags.txt
