## Process this file with automake to produce Makefile.in

SUBDIRS = po src sounds

EXTRA_DIST = \
	autogen.sh\
	gtktetcolor.soundlist\
	pixmaps/pause.xpm\
	pixmaps/scores.xpm\
	pixmaps/README\
	intltool-extract.in \
	intltool-merge.in    \
	intltool-update.in \
	gtktetcolor.spec.in \
	gtktetcolor.spec.in

if use_gnome
soundlist_DATA = gtktetcolor.soundlist
soundlistdir = $(sysconfdir)/sound/events
scoredir=$(localstatedir)/games
install-data-local:
	-$(mkinstalldirs) $(DESTDIR)$(scoredir)
	touch $(DESTDIR)$(scoredir)/gtktetcolor.scores
	-chown games.games $(DESTDIR)$(scoredir)/gtktetcolor.scores
	-chmod 664 $(DESTDIR)$(scoredir)/gtktetcolor.scores
	-chgrp games $(DESTDIR)$(bindir)/gtktetcolor && chmod 2111 $(DESTDIR)$(bindir)/gtktetcolor
else
install-score:
	cd ./src && $(MAKE) $(AM_MAKEFLAGS) install-score
endif
