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

#include "main.h"
#include <time.h>
#include <sys/stat.h>
// #ifdef ENABLE_NLS
// # include <locale.h>
// #endif

#include "interface.h"
#include "callbacks.h"
#include "score.h"
#include "game.h"
#include "preferences.h"
#include "pixmaps.h"
#include "gtktetcolor.xpm"

// defaults
gint cell_width    = 35;
gint initial_level = 1;
gint max_score     = 0;
gint destroy_delay = 100;
///#ifdef USE_GNOME
gboolean sound_on = 0;
///#endif


#define P_DIR "games"
char * get_config_dir_file (const char * file)
{
   /* returns a path that must be freed with g_free */
   char * config_home, * res;
#if __MINGW32__
   config_home = getenv ("LOCALAPPDATA"); /* XP */
   if (!config_home) {
      config_home = getenv ("APPDATA");
   }
#else
   config_home = getenv ("XDG_CONFIG_HOME");
#endif
   if (config_home) {
      res = g_build_filename (config_home, P_DIR, file, NULL);
   } else {
      res = g_build_filename (g_get_home_dir(), ".config", P_DIR, file, NULL);
   }
   return res;
}

/* ============================================================================= */

int main (int argc, char *argv[])
{
#ifdef ENABLE_NLS
   bindtextdomain (GETTEXT_PACKAGE, LOCALEDIR);
   bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
   textdomain (GETTEXT_PACKAGE);
#endif

   gtk_init (&argc, &argv);

   /* Make sure confdir exists */
   char * confdir = get_config_dir_file (NULL);
   g_mkdir_with_parents (confdir, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
   g_free (confdir);

   load_preferences ();

   main_window = create_main_window ();
   create_pixmaps (drawingarea, 1);

   icon_xpm = gdk_pixbuf_new_from_xpm_data ((const char **) &gtktetcolor_xpm);
   gtk_window_set_icon (GTK_WINDOW (main_window), icon_xpm);
   g_object_unref (G_OBJECT (icon_xpm));

   srand (time (0));

   init_game (main_window);

   gtk_main ();
   before_exit ();

   return 0;
}

/* ============================================================================= */

void before_exit (void)
{
   gint i;
   for (i = 0; i < NUMBER_COLORS + 1; i++) {
      if (colors[i])
         g_object_unref (colors[i]);
   }
   free_pixmap_chars ();
}
