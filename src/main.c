/* GtkTetcolor
 * Copyright (C) 1999 - 2002 Andrey V. Panov
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#ifdef USE_GNOME
# include <gnome.h>
# include <gconf/gconf-client.h>
#else
# include <gtk/gtk.h>
#endif
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <sys/stat.h>
#include <gdk/gdkkeysyms.h>
#include <glib/gi18n.h>
// #ifdef ENABLE_NLS
// # include <locale.h>
// #endif

#include "interface.h"
#include "main.h"
#include "callbacks.h"
#include "score.h"
#include "game.h"
#include "preferences.h"
#include "pixmaps.h"

gint stop;

/* Default settings */
gint cell_width = 35, initial_level = 1, max_score = 0, destroy_delay = 100;
#ifdef WIN32
gint use_graykeys =  1; 
#else  
gint use_graykeys =  0; 
#endif

#ifdef USE_GNOME
gboolean sound_on = 0;
GConfClient *gconf_client = NULL;
#else
gboolean text_toolbar = 0;
#endif
gint nav_keys[4], alt_nav_keys[4];
gchar *font_name;

GdkPixbuf *colors[NUMBER_COLORS + 1], *border_w_gdkxpm, *border_h_gdkxpm;
char *border_width_xpm[2 + border];
char *border_height_xpm[2 + MAX_CELL_SIZE * Y_SIZE];
gchar *label_name[MAX_LABEL];
GdkPixbuf *icon_xpm;

int
main (int argc, char *argv[])
{
#include "gtktetcolor.xpm"

#ifdef ENABLE_NLS
  bindtextdomain (GETTEXT_PACKAGE, LOCALEDIR);
  bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
  textdomain (GETTEXT_PACKAGE);
#endif

#ifdef USE_GNOME
  gnome_program_init (PACKAGE, VERSION, LIBGNOMEUI_MODULE,
		      argc, argv, GNOME_PARAM_APP_DATADIR, DATADIR, NULL);
  gconf_client = gconf_client_get_default ();
  gconf_client_add_dir (gconf_client, "/apps/gtktetcolor/Option",
			GCONF_CLIENT_PRELOAD_NONE, NULL);
/*
  gconf_client_notify_add (gconf_client, "/apps/gtktetcolor/Option",                          
    key_changed_callback, NULL, NULL, NULL);                                                        
  */

#else
  gtk_init (&argc, &argv);
#endif

  load_preferences ();

  main_window = create_main_window ();

  if (use_graykeys) {
    nav_keys[0] = GDK_Right;
    nav_keys[1] = GDK_Left;
    nav_keys[2] = GDK_Up;
    nav_keys[3] = GDK_Down;
    alt_nav_keys[0] = GDK_Right;
    alt_nav_keys[1] = GDK_Left;
    alt_nav_keys[2] = GDK_Up;
    alt_nav_keys[3] = GDK_Down;
  }
  else {
    nav_keys[0] = GDK_KP_6;
    nav_keys[1] = GDK_KP_4;
    nav_keys[2] = GDK_KP_8;
    nav_keys[3] = GDK_KP_5;
    alt_nav_keys[0] = GDK_KP_Right;
    alt_nav_keys[1] = GDK_KP_Left;
    alt_nav_keys[2] = GDK_KP_Up;
    alt_nav_keys[3] = GDK_KP_Begin;
  }

  create_pixmaps (drawingarea, 1);

  icon_xpm = gdk_pixbuf_new_from_xpm_data ((const char **) &gtktetcolor_xpm);
  gtk_window_set_icon (GTK_WINDOW (main_window), icon_xpm);

  srand (time (0));

  init_game (main_window);

  gtk_main ();
  before_exit ();
  if (icon_xpm)
    g_object_unref (G_OBJECT (icon_xpm));
#ifdef USE_GNOME
  g_object_unref (G_OBJECT (gconf_client));
#endif
  return 0;
}

void
before_exit (void)
{
  gint i;

  for (i = 0; i < NUMBER_COLORS + 1; i++) {
    if (colors[i])
      g_object_unref (colors[i]);
  }
  if (border_w_gdkxpm)
    g_object_unref (border_w_gdkxpm);
  if (border_h_gdkxpm)
    g_object_unref (border_h_gdkxpm);

  free_pixmap_chars ();
}
