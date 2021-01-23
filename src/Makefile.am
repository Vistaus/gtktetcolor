## Process this file with automake to produce Makefile.in

Gamesdir = $(datadir)/applications
icondir = $(datadir)/pixmaps
Games_DATA = gtktetcolor.desktop
icon_DATA = gtktetcolor.xpm

AM_CPPFLAGS = \
	-DDATADIR=\"$(datadir)\" -DLOCALEDIR=\"$(datadir)/locale\" \
	@GTK_CFLAGS@ @GNOME_CFLAGS@

bin_PROGRAMS = gtktetcolor

gtktetcolor_SOURCES = \
	main.c\
	score.c\
	interface.c\
	callbacks.c\
	preferences.c\
	game.c\
	pixmaps.c

gtktetcolor_LDADD = @GTK_LIBS@ @GNOME_LIBS@
