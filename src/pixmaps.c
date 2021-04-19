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

//GdkPixbuf *colors[NUMBER_COLORS + 1];
static char *color[NUMBER_COLORS + 1][MAX_CELL_SIZE + 3];

gboolean create_pixmaps (GtkWidget * widget, gint first_time)
{
   gint i, j;
   gchar cell_size_char[20];
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

   for (i = 0; i < NUMBER_COLORS + 1; i++) {
      if (colors[i]) {
         g_object_unref (colors[i]);
      }
      colors[i] = gdk_pixbuf_new_from_xpm_data ((const char **) &color[i]);
   }

   gtk_widget_set_size_request (widget, X_SIZE * cell_width + 2 * border,
                                Y_SIZE * cell_width + 2 * border);
   return TRUE;
}


gboolean redraw_cells ()
{
   gint i, j, cell_state_;
   int xdest, ydest;

   GdkWindow * gsurface = gtk_widget_get_window (drawingarea);
   cairo_t * cr = gdk_cairo_create (gsurface);

   for (j = 0; j < Y_SIZE; j++)
   {
      for (i = 0; i < X_SIZE; i++)
      {
         cell_state_ = cell_state[i][j];
         if (cell_state_prev[i][j] != cell_state_)
         {
            xdest = border + i * cell_width;
            ydest = border + j * cell_width;
            gdk_cairo_set_source_pixbuf (cr, colors[cell_state_],
                                         xdest, ydest);
            cairo_paint (cr);
            cell_state_prev[i][j] = cell_state_;
         }
      }
   }

   cairo_destroy (cr);
   return FALSE;
}


gboolean redraw_all_cells ()
{
   gint i, j;
   int xdest, ydest;

   GdkWindow * gsurface = gtk_widget_get_window (drawingarea);
   cairo_t * cr = gdk_cairo_create (gsurface);

   int width  = gtk_widget_get_allocated_width (drawingarea);
   int height = gtk_widget_get_allocated_height (drawingarea);

   // red border
   cairo_set_source_rgba (cr, 1.0, 0.0, 0.0, 1.0);
   cairo_set_line_width (cr, 4.0);
   cairo_rectangle (cr, 0.0, 0.0, width, height);
   cairo_stroke (cr);

   for (j = 0; j < Y_SIZE; j++)
   {
      for (i = 0; i < X_SIZE; i++)
      {
         xdest = border + i * cell_width;
         ydest = border + j * cell_width;
         gdk_cairo_set_source_pixbuf (cr, colors[cell_state[i][j]],
                                      xdest, ydest);
         cairo_paint (cr);
      }
   }

   cairo_destroy (cr);
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
}
