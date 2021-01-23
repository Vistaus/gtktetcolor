## Process this file with automake to produce Makefile.in

Gamesdir = $(datadir)/applications
icondir = $(datadir)/pixmaps
Games_DATA = gtktetcolor.desktop
icon_DATA = gtktetcolor.xpm
scoredir=${datadir}/gtktetcolor

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

install-score:
	-$(mkinstalldirs) $(DESTDIR)$(scoredir)
	touch $(DESTDIR)$(scoredir)/gtktetcolor_score
	-chown games.games $(DESTDIR)$(scoredir)/gtktetcolor_score
	-chmod 664 $(DESTDIR)$(scoredir)/gtktetcolor_score
	-chgrp games $(DESTDIR)$(bindir)/gtktetcolor && chmod 2111 $(DESTDIR)$(bindir)/gtktetcolor
