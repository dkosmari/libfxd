#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.70])
AC_INIT([libfxd], [0.2], [])
AC_CONFIG_SRCDIR([include/fxd/fixed.hpp])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE([foreign])

# Checks for programs.
AC_PROG_CC
AC_PROG_CXX
AC_PROG_RANLIB

AC_LANG([C++])

AX_APPEND_COMPILE_FLAGS([-std=c++20], [CXX])
# AX_APPEND_COMPILE_FLAGS([-std=gnu++20], [CXXFLAGS])

# Checks for libraries.

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.


AC_CHECK_PROGS([DOXYGEN],
               [doxygen],
               [FOUND_DOXYGEN=no])
AM_CONDITIONAL([HAVE_DOXYGEN], [test "x$FOUND_DOXYGEN" != "xno"])



AC_CONFIG_FILES([Makefile
                 doc/Doxyfile
                 doc/Makefile
                 examples/Makefile
                 include/fxd/Makefile
                 tests/Makefile
                 tests/catch2/Makefile])
AC_OUTPUT
