# tests/Makefile.am


noinst_HEADERS = \
	printer.hpp \
	rng.hpp \
	rounder.hpp \
	setup.hpp \
	test-types.hpp


SUBDIRS = catch2


AM_CPPFLAGS = \
	-I$(top_srcdir)/include


AM_CXXFLAGS = \
	-Wall -Wextra -Werror \
	-frounding-math \
	-fsanitize=undefined \
	-fsanitize-undefined-trap-on-error \
	--coverage


check_PROGRAMS = \
	test-add \
	test-conv \
	test-div \
	test-limits \
	test-mul \
	test-safe \
	test-sub


AM_DEFAULT_SOURCE_EXT = .cpp


LDADD = \
	catch2/libcatch2.la


TESTS = $(check_PROGRAMS)

CLEANFILES = *.gcda *.gcno *.info

clean-local:
	$(RM) -r html

.PHONY: cover-prepare cover-process coverage

cover-prepare: $(check_PROGRAMS)
	$(RM) *.info
	lcov --zerocounters --directory .
	lcov \
		--capture \
		--initial \
		--exclude "/usr/include/*" \
		--exclude "$(abs_srcdir)/catch2/*" \
		--exclude "$(abs_srcdir)/*.hpp" \
		--directory . \
		--base-directory $(srcdir) \
		--output-file coverage-initial.info

cover-process: check-TESTS
	lcov \
		--capture \
		--exclude "/usr/include/*" \
		--exclude "$(abs_srcdir)/catch2/*" \
		--exclude "$(abs_srcdir)/*.hpp" \
		--directory . \
		--base-directory $(srcdir) \
		--output-file coverage-tests.info
	lcov \
		--add-tracefile coverage-initial.info \
		--add-tracefile coverage-tests.info \
		--output-file coverage.info
	$(RM) -r html
	mkdir html
	genhtml --demangle-cpp --output-directory html coverage.info

coverage: cover-prepare cover-process


.PHONY: company

company: compile_flags.txt

compile_flags.txt: Makefile
	echo "$(AM_CPPFLAGS) $(CPPFLAGS) $(DEFS)" | xargs -n1 | sort -u > $(srcdir)/compile_flags.txt
