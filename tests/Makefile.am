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


if USE_SANITIZE
SANITIZE_FLAGS = -fsanitize=undefined -fsanitize-undefined-trap-on-error
else !USE_SANITIZE
SANITIZE_FLAGS =
endif !USE_SANITIZE


if USE_COVERAGE
COVERAGE_FLAGS = --coverage -fno-inline
else !USE_COVERAGE
COVERAGE_FLAGS =
endif !USE_COVERAGE


AM_CXXFLAGS = \
	-Wall -Wextra -Werror \
	-frounding-math \
	$(SANITIZE_FLAGS) \
	$(COVERAGE_FLAGS)


check_PROGRAMS = \
	test-add \
	test-conv \
	test-div \
	test-limits \
	test-mul \
	test-safe \
	test-sub


AM_DEFAULT_SOURCE_EXT = .cpp


LDADD = catch2/libcatch2.la


TESTS = $(check_PROGRAMS)


.PHONY: company coverage coverage-zero


company: compile_flags.txt

compile_flags.txt: Makefile
	echo "$(AM_CPPFLAGS) $(CPPFLAGS) $(DEFS)" | xargs -n1 | sort -u > $(srcdir)/compile_flags.txt


if USE_COVERAGE

MOSTLYCLEANFILES = \
	*.gcda \
	*.gcno \
	coverage-init.info \
	coverage-tests.info

LCOV_EXCLUDES = \
	--exclude "/usr/include/*" \
	--exclude "$(abs_srcdir)/catch2/*" \
	--exclude "$(abs_srcdir)/*.hpp"

LCOV_FLAGS = \
	$(LCOV_EXCLUDES) \
	--directory . \
	--base-directory $(srcdir)

clean-local:
	$(RM) -r html

coverage-zero:
	$(LCOV) --zerocounters --directory .

coverage-init.info: coverage-zero check
	$(LCOV) \
		--capture \
		--initial \
		$(LCOV_FLAGS) \
		--output-file $@

coverage-tests.info: check
	$(LCOV) \
		--capture \
		$(LCOV_FLAGS) \
		--output-file $@

coverage: coverage-init.info coverage-tests.info
	$(RM) -r html
	mkdir html
	$(GENHTML) \
		--demangle-cpp \
		--output-directory html \
		$^

endif USE_COVERAGE


