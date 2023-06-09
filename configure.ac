#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.70])
AC_INIT([libfxd], [0.2.dev], [])
AC_CONFIG_SRCDIR([include/fxd/fixed.hpp])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE([foreign])

LT_INIT([disable-shared])
LT_LANG([C++])

# Checks for programs.
AC_PROG_CC
AC_PROG_CPP
AC_PROG_CXX
AC_CHECK_PROGS([LCOV], [lcov], [MISSING_LCOV=yes])
AC_CHECK_PROGS([GENHTML], [genhtml], [MISSING_LCOV=yes])

AC_LANG([C++])
AX_APPEND_COMPILE_FLAGS([-std=c++20], [CXX])

# Checks for libraries.

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.

AC_CHECK_PROGS([DOXYGEN], [doxygen])
AM_CONDITIONAL([USE_DOXYGEN], [test -n "${DOXYGEN}"])


PKG_CHECK_MODULES([BENCHMARK], [benchmark],
                  [BENCHMARK_LIBS="${BENCHMARK_LIBS}"],
                  [MISSING_BENCHMARK=yes])
AM_CONDITIONAL([USE_BENCHMARK], [test "x${MISSING_BENCHMARK}" != "xyes"])


# --enable-coverage option
AC_ARG_ENABLE([coverage],
              [AS_HELP_STRING([--enable-coverage],
              [enable coverage tests (useful for developers)])])
AS_VAR_IF([MISSING_LCOV], [yes],
          [
                AS_VAR_IF([enable_coverage], [yes],
                          [AC_MSG_WARN([Can't enable coverage because LCOV is missing.])])
                enable_coverage=no
          ])
AS_VAR_IF([enable_coverage], [yes],
          [],
          [AS_VAR_SET([enable_coverage], [no])])

AM_CONDITIONAL([USE_COVERAGE], [test ${enable_coverage} = "yes"])


# --enable-sanitize option
AC_ARG_ENABLE([sanitize],
              [AS_HELP_STRING([--enable-sanitize],
                              [build tests with sanitization (useful for developers)])])
AS_VAR_IF([enable_sanitize], [yes],
          [],
          [AS_VAR_SET([enable_sanitize], [no])])

AM_CONDITIONAL([USE_SANITIZE], [test "x${enable_sanitize}" = "xyes"])


# report options
AC_MSG_NOTICE([The following options will be used for tests (when `make check`):])
AC_MSG_NOTICE([    coverage: ${enable_coverage}])
AC_MSG_NOTICE([    sanitize: ${enable_sanitize}])


AC_CONFIG_FILES([Makefile
                 benchmarks/Makefile
                 doc/Doxyfile
                 doc/Makefile
                 examples/Makefile
                 include/fxd/Makefile
                 tests/Makefile
                 tests/catch2/Makefile])
AC_OUTPUT
