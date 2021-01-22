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

INCLUDES = \
	-DDATADIR=\""$(datadir)"\"\
	-DGTKTETCOLOR_LOCALE_DIR=\""$(prefix)/$(DATADIRNAME)/locale"\" \
	@GTKTETCOLOR_CFLAGS@

##	-DGTK_DISABLE_DEPRECATED -DGNOME_DISABLE_DEPRECATED \

bin_PROGRAMS = gtktetcolor

gtktetcolor_SOURCES = \
	main.c\
	score.c\
	interface.c\
	callbacks.c\
	preferences.c\
	game.c\
	pixmaps.c

noinst_HEADERS = \
	main.h\
	score.h\
	interface.h\
	callbacks.h\
	preferences.h\
	game.h\
	pixmaps.h\
	gtktetcolor.xpm

EXTRA_DIST = gtktetcolor.desktop.in

gtktetcolor_LDADD = @GTKTETCOLOR_LIBS@

#scoredir=${datadir}/gtktetcolor
#if use_gnome
#install-data-local:
#	-$(mkinstalldirs) $(DESTDIR)$(scoredir)
#	touch $(DESTDIR)$(scoredir)/gtktetcolor_score
#	-chown games.games $(DESTDIR)$(scoredir)/gtktetcolor_score
#	-chmod 664 $(DESTDIR)$(scoredir)/gtktetcolor_score
#	-chgrp games $(DESTDIR)$(bindir)/gtktetcolor && chmod 2111 $(DESTDIR)$(bindir)/gtktetcolor
#endif
install-score:
	-$(mkinstalldirs) $(DESTDIR)$(scoredir)
	touch $(DESTDIR)$(scoredir)/gtktetcolor_score
	-chown games.games $(DESTDIR)$(scoredir)/gtktetcolor_score
	-chmod 664 $(DESTDIR)$(scoredir)/gtktetcolor_score
	-chgrp games $(DESTDIR)$(bindir)/gtktetcolor && chmod 2111 $(DESTDIR)$(bindir)/gtktetcolor
