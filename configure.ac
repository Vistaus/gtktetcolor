# Process this file with autoconf to produce a configure script.

AC_INIT([gtktetcolor], [0.6.5pre3])
AM_CONFIG_HEADER([config.h])
AM_INIT_AUTOMAKE([-Wall foreign])
AC_CONFIG_MACRO_DIR([m4])
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])
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

gnome=yes
AC_ARG_ENABLE([gnome],
[  --disable-gnome         disable use of gnome],
	[gnome=$enableval], [gnome=yes])

if test "$gnome" = yes; then
  PKG_CHECK_MODULES(GNOME, libgnomeui-2.0 >= 2.0.0 gconf-2.0 >= 1.2.0, [
    AC_DEFINE(USE_GNOME,[] ,[Gnome use])
  ], gnome=no)
fi
AM_CONDITIONAL(use_gnome, test x$gnome = xyes)
AC_SUBST(GNOME_CFLAGS)
AC_SUBST(GNOME_LIBS)

#========================================================================

case "$CC" in
   gcc*) CPPFLAGS="$CPPFLAGS -Wall -Wextra -Wno-unused-parameter -Wno-missing-field-initializers" ;;
   *) ;;
esac

AC_OUTPUT([
Makefile
src/Makefile
sounds/Makefile
po/Makefile.in
])
