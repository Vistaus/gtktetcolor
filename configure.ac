#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_INIT([gtktetcolor], [0.6.5pre3], [github.com/wdlkmpx/gtktetcolor])
AC_CONFIG_AUX_DIR([autoconf])
AC_CONFIG_MACRO_DIR([autoconf-m4])
AM_INIT_AUTOMAKE([-Wall foreign])
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

AC_CONFIG_HEADER(config.h)
AM_GNU_GETTEXT([external])
m4_ifdef([AM_PROG_AR], [AM_PROG_AR])
m4_ifdef([AM_MAINTAINER_MODE], [AM_MAINTAINER_MODE])

LT_INIT

# Init intltool
IT_PROG_INTLTOOL

# Checks for programs.
AC_PROG_CC

#========================================================================

GTK2_CHECK="gtk+-2.0 >= 2.14.0"
PKG_CHECK_MODULES([GTK], [$GTK2_CHECK], [], [ERROR: install GTK2])
AC_SUBST([GTK_CFLAGS])
AC_SUBST([GTK_LIBS])

#========================================================================

case "$CC" in
   gcc*) CPPFLAGS="$CPPFLAGS -Wall -Wextra -Wno-unused-parameter -Wno-missing-field-initializers" ;;
   *) ;;
esac

GETTEXT_PACKAGE=gtktetcolor
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE,"$GETTEXT_PACKAGE", [Gettext package.])

AM_GLIB_GNU_GETTEXT


AC_OUTPUT([
Makefile
src/Makefile
sounds/Makefile
po/Makefile.in
])
