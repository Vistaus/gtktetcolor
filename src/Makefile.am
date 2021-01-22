## Process this file with automake to produce Makefile.in
if use_gnome
Gamesdir = $(datadir)/applications
soundlistdir = $(sysconfdir)/sound/events
icondir = $(datadir)/pixmaps
Games_in_files = gtktetcolor.desktop.in
Games_DATA = $(Games_in_files:.desktop.in=.desktop)
@INTLTOOL_DESKTOP_RULE@

## Games_DATA = gtktetcolor.desktop
icon_DATA = gtktetcolor.xpm
else
scoredir=${datadir}/gtktetcolor
endif

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

EXTRA_DIST = gtktetcolor.desktop.in

gtktetcolor_LDADD = @GTK_LIBS@ @GNOME_LIBS@

install-score:
	-$(mkinstalldirs) $(DESTDIR)$(scoredir)
	touch $(DESTDIR)$(scoredir)/gtktetcolor_score
	-chown games.games $(DESTDIR)$(scoredir)/gtktetcolor_score
	-chmod 664 $(DESTDIR)$(scoredir)/gtktetcolor_score
	-chgrp games $(DESTDIR)$(bindir)/gtktetcolor && chmod 2111 $(DESTDIR)$(bindir)/gtktetcolor
