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

#include "interface.h"
#include "callbacks.h"
#include "score.h"
#include "game.h"
#include "pixmaps.h"

gint stop;
gint cell_width, initial_level, max_score, use_graykeys;
gint nav_keys[4], alt_nav_keys[4];
GdkFont *font;
gchar *font_name;

GdkPixbuf *colors[NUMBER_COLORS + 1], *border_w_gdkxpm, *border_h_gdkxpm;
static char *color[NUMBER_COLORS + 1][MAX_CELL_SIZE + 3];
char *border_width_xpm[2 + border];
char *border_height_xpm[2 + MAX_CELL_SIZE * Y_SIZE];
gchar *label_name[MAX_LABEL];


gboolean create_pixmaps (GtkWidget * widget, gint first_time)
{
   gint i, j;
   gchar cell_size_char[20];
   gchar border_line_width[cell_width * X_SIZE + 2 * border + 1];
   gchar cell_core[MAX_CELL_SIZE + 1], cell_border[MAX_CELL_SIZE + 1];
   static gchar *color_name[NUMBER_COLORS + 1] = { "gray25",
      "blue",
      "green",
      "cyan",
      "magenta",
      "red",
      "yellow",
      "black"
   };

   /* prepare strings */
   g_snprintf (cell_size_char, 20, "%d %d 2 1", cell_width, cell_width);
   for (j = 0; j < cell_width; j++) {
      cell_border[j] = '+';
   }
   cell_border[cell_width] = '\0';
   cell_core[0] = '+';
   cell_core[cell_width - 1] = '+';
   cell_core[cell_width] = '\0';
   for (j = 1; j < cell_width - 1; j++) {
      cell_core[j] = '@';
   }

   /* allocating memory for pixmaps */
   if (first_time)
   {
      color[0][0] = (gchar *) g_malloc (MAX_CELL_SIZE + 1);
      color[0][1] = (gchar *) g_malloc (MAX_CELL_SIZE + 1);
      color[0][3] = (gchar *) g_malloc (MAX_CELL_SIZE + 1);
      color[1][1] = (gchar *) g_malloc (MAX_CELL_SIZE + 1);
      color[1][4] = (gchar *) g_malloc (MAX_CELL_SIZE + 1);
      for (i = 0; i < NUMBER_COLORS + 1; i++)
         color[i][2] = (gchar *) g_malloc (MAX_CELL_SIZE + 1);
      border_width_xpm[0] = (gchar *) g_malloc (20);
      border_width_xpm[1] = (gchar *) g_malloc (20);
      border_height_xpm[0] = (gchar *) g_malloc (20);
      border_width_xpm[2] = (gchar *) g_malloc (MAX_CELL_SIZE * X_SIZE + 2 * border + 1);
      border_height_xpm[2] = (gchar *) g_malloc (border + 1);

      strcpy (color[0][1], "       c None");
      strcpy (color[0][2], "+      c gray25");

      strcpy (color[1][1], "+      c thistle4");
      for (i = 2; i < NUMBER_COLORS + 1; i++) {
         color[i][1] = color[1][1];
      }
      for (i = 1; i < NUMBER_COLORS + 1; i++) {
         color[i][0] = color[0][0];
         strcpy (color[i][2], "@      c ");
         strcat (color[i][2], color_name[i]);
      }

      strcpy (border_width_xpm[1], ".      c gold");
      border_height_xpm[1] = border_width_xpm[1];
      strcpy (border_height_xpm[2], "..");
   }

   /* background pixmap */
   strcpy (color[0][0], cell_size_char);
   strcpy (color[0][3], cell_border);
   for (i = 4; i < cell_width + 3; i++) {
      color[0][i] = color[0][3];
   }

   /* colored pixmaps */
   strcpy (color[1][4], cell_core);
   for (i = 1; i < NUMBER_COLORS + 1; i++) {
      color[i][3] = color[0][3];
      if (i != 1) {
         color[i][4] = color[1][4];
      }
      for (j = 5; j < cell_width + 2; j++) {
         color[i][j] = color[1][4];
      }
      color[i][cell_width + 2] = color[0][3];
   }
   for (i = 0; i < NUMBER_COLORS + 1; i++) {
      for (j = cell_width + 3; j < MAX_CELL_SIZE + 3; j++) {
         color[i][j] = NULL;
      }
   }

   /* border pixmaps */
   g_snprintf (border_width_xpm[0], 20, "%d %d 1 1",
   cell_width * X_SIZE + 2 * border, border);
   for (i = 0; i < cell_width * X_SIZE + 2 * border; i++) {
      border_line_width[i] = '.';
   }
   border_line_width[cell_width * X_SIZE + 2 * border] = '\0';
   strcpy (border_width_xpm[2], border_line_width);
   border_width_xpm[3] = border_width_xpm[2];

   g_snprintf (border_height_xpm[0], 20, "%d %d 1 1",
   border, cell_width * Y_SIZE);
   for (i = 3; i < cell_width * Y_SIZE + 2; i++) {
      border_height_xpm[i] = border_height_xpm[2];
   }

   for (i = 0; i < NUMBER_COLORS + 1; i++) {
      if (colors[i]) {
         g_object_unref (colors[i]);
      }
      colors[i] = gdk_pixbuf_new_from_xpm_data ((const char **) &color[i]);
   }

   if (border_w_gdkxpm) {
      g_object_unref (border_w_gdkxpm);
   }
   border_w_gdkxpm = gdk_pixbuf_new_from_xpm_data ((const char **) &border_width_xpm);

   if (border_h_gdkxpm) {
      g_object_unref (border_h_gdkxpm);
   }
   border_h_gdkxpm = gdk_pixbuf_new_from_xpm_data ((const char **) &border_height_xpm);
   gtk_widget_set_size_request (widget, X_SIZE * cell_width + 2 * border,
                                Y_SIZE * cell_width + 2 * border);
   return TRUE;
}


gboolean redraw_cells ()
{
   gint i, j, cell_state_;

   for (j = 0; j < Y_SIZE; j++)
   {
      for (i = 0; i < X_SIZE; i++) {
         cell_state_ = cell_state[i][j];
         if (cell_state_prev[i][j] != cell_state_)
         {
            gdk_draw_pixbuf (drawingarea->window,
                             drawingarea->style->fg_gc[gtk_widget_get_state (drawingarea)],
                             colors[cell_state_],
                             0, 0,
                             border + i * cell_width,
                             border + j * cell_width, cell_width,
                             cell_width, GDK_RGB_DITHER_NONE, 0, 0);
            cell_state_prev[i][j] = cell_state_;
         }
      }
   }
   return FALSE;
}


gboolean redraw_all_cells ()
{
   gint i, j;

   for (i = 0; i < 2; i++) {
      gdk_draw_pixbuf (drawingarea->window,
                       drawingarea->style->fg_gc[gtk_widget_get_state (drawingarea)],
                       border_w_gdkxpm,
                       0, 0,
                       0, i * (border + Y_SIZE * cell_width),
                       cell_width * X_SIZE + 2 * border, border,
                       GDK_RGB_DITHER_NONE, 0, 0);
   }

   for (i = 0; i < 2; i++) {
      gdk_draw_pixbuf (drawingarea->window,
                       drawingarea->style->fg_gc[gtk_widget_get_state (drawingarea)],
                       border_h_gdkxpm,
                       0, 0,
                       i * (X_SIZE * cell_width + border), border,
                       border, cell_width * Y_SIZE,
                       GDK_RGB_DITHER_NONE, 0, 0);
   }

   for (j = 0; j < Y_SIZE; j++) {
      for (i = 0; i < X_SIZE; i++) {
         gdk_draw_pixbuf (drawingarea->window,
                          drawingarea->style->fg_gc[gtk_widget_get_state (drawingarea)],
                          colors[cell_state[i][j]], 0,
                          0, border + i * cell_width,
                          border + j * cell_width, cell_width, cell_width,
                          GDK_RGB_DITHER_NONE, 0, 0);
      }
   }
   return FALSE;
}


/* freeing memory of pixmaps */
void free_pixmap_chars (void)
{
   int i;

   g_free (color[0][0]);
   g_free (color[0][1]);
   g_free (color[0][3]);
   g_free (color[1][1]);
   g_free (color[1][4]);
   for (i = 0; i < NUMBER_COLORS + 1; i++) {
      g_free (color[i][2]);
   }
   g_free (border_width_xpm[0]);
   g_free (border_width_xpm[1]);
   g_free (border_height_xpm[0]);
   g_free (border_width_xpm[2]);
   g_free (border_height_xpm[2]);
}
