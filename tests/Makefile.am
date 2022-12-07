# tests/Makefile.am


noinst_HEADERS = printer.hpp


SUBDIRS = catch2


AM_CPPFLAGS = \
	-I$(top_srcdir)/include


AM_CXXFLAGS = \
	-Wall -Wextra \
	-frounding-math \
	-fsanitize=undefined

check_PROGRAMS = \
	test-add \
	test-div \
	test-mul


AM_DEFAULT_SOURCE_EXT = .cpp


LDADD = \
	catch2/libcatch2.a


TESTS = $(check_PROGRAMS)


