## Process this file with automake to produce Makefile.in

SUBDIRS = po src sounds

ACLOCAL_AMFLAGS = -I m4

EXTRA_DIST = \
	autogen.sh \
	gtktetcolor.soundlist \
	pixmaps/pause.xpm \
	pixmaps/scores.xpm \
	pixmaps/README

if use_gnome
soundlist_DATA = gtktetcolor.soundlist
soundlistdir = $(sysconfdir)/sound/events
endif
