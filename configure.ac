#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([cdippy-cli], [0.0.1], [aquilairreale@ymail.com])
AM_INIT_AUTOMAKE([foreign subdir-objects dejagnu])
AC_CONFIG_SRCDIR([src/main.c])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([Makefile])

# Checks for programs.
AC_PROG_CC_C99

AC_PROG_LEX
if test "x$LEX" != xflex; then
    AC_MSG_ERROR([flex is required to build this software])
fi

AC_PROG_YACC
if test "x$YACC" != 'xbison -y'; then
    AC_MSG_ERROR([bison is required to build this software])
fi

# Checks for libraries.
AC_CHECK_LIB([readline], [readline])

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_MALLOC

AC_OUTPUT
