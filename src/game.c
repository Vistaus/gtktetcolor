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
gint cell_width, initial_level, max_score, use_graykeys, destroy_delay;
gint current_score_number;
#ifdef USE_GNOME
gboolean sound_on;
#endif
gint nav_keys[4], alt_nav_keys[4];
gchar *font_name;

gchar *label_name[MAX_LABEL];


void init_game (GtkWidget * widget)
{
   gchar num[2], timer_buf[5] = "0:00";
   GtkWidget *label;
   GtkWidget *score_label_number, *bonus_label_number, *bonus_label;
   gchar score_text[10];
   gint i, j;
   GtkWidget *max_score_number_label;
   gchar max_score_text[10];

   level = initial_level;
   lines = 0;
   score = 0;
   level_tic = 0;
   continue_game = 1;
   bonus_tic = 0;
   interval = INI_INTERVAL;
   current_score_number = 10;	/* 11th position in top ten */

   read_score ();
   max_score = saved_score[0];

   max_score_number_label = (GtkWidget *) g_object_get_data (G_OBJECT (main_window), label_name[7]);
   g_snprintf (max_score_text, 10, "%d", max_score);
   gtk_label_set_text (GTK_LABEL (max_score_number_label), max_score_text);
   for (j = 0; j < Y_SIZE; j++) {
      for (i = 0; i < X_SIZE; i++) {
         cell_state[i][j] = 0;
         cell_state_wo_block[i][j] = 0;
         removing_cell[i][j] = 0;
      }
   }

   label = (GtkWidget *) g_object_get_data (G_OBJECT (main_window), label_name[3]);
   score_label_number = (GtkWidget *) g_object_get_data (G_OBJECT (main_window), label_name[5]);
   bonus_label_number = (GtkWidget *) g_object_get_data (G_OBJECT (main_window), label_name[1]);
   bonus_label = (GtkWidget *) g_object_get_data (G_OBJECT (main_window), label_name[0]);
   gtk_widget_set_style (score_label_number, gtk_widget_get_style (bonus_label_number));
   redraw_cells ();
   if (timeout) {
      g_source_remove (timeout);
   }
   if (level_timeout) {
      g_source_remove (level_timeout);
   }
   level_timeout = g_timeout_add (LEVEL_INT, (GSourceFunc) change_level, main_window);
   g_snprintf (num, 2, "%d", level);
   gtk_label_set_text (GTK_LABEL (label), num);
   g_snprintf (score_text, 10, "%d", score);
   gtk_label_set_text (GTK_LABEL (score_label_number), score_text);
   gtk_label_set_text (GTK_LABEL (bonus_label_number), "");
   gtk_label_set_text (GTK_LABEL (bonus_label), "");
   new_block ();
   redraw_cells ();
   label = (GtkWidget *) g_object_get_data (G_OBJECT (main_window), label_name[8]);
   gtk_label_set_text (GTK_LABEL (label), timer_buf);
}


gboolean new_block ()
{
   gint i;

   interval = INI_INTERVAL - INI_INTERVAL * (level - 1) / 10;
   timeout = g_timeout_add (interval, (GSourceFunc) timeout_callback, main_window);
   if (cell_state_wo_block[3][0]) {
      game_over (main_window);
      return FALSE;
   }
   block.type = rand () % NUMBER_TYPES;
   /* Initial location of new block */
   block.x[0] = 3;
   block.y[0] = 0;
   if (block.type)
   {
      if (cell_state_wo_block[3][1]) {
         game_over (main_window);
         return FALSE;
      }
      block.x[1] = 3;
      block.y[1] = 1;
      switch (block.type)
      {
         case 1:
            block.num = 2;
            break;
         case 2:
            if (cell_state_wo_block[3][2]) {
               game_over (main_window);
               return FALSE;
            }
            block.x[2] = 3;
            block.y[2] = 2;
            block.num = 3;
            break;
         case 3:
            if (cell_state_wo_block[2][1]) {
               game_over (main_window);
               return FALSE;
            }
            block.x[2] = 2;
            block.y[2] = 1;
            block.num = 3;
            break;
      }
   }
   else {
      block.num = 1;
   }

   for (i = 0; i < block.num; i++) {
      block.color[i] = (rand () % (NUMBER_COLORS - 1)) + 1;
      cell_state[block.x[i]][block.y[i]] = block.color[i];
   }
   return TRUE;
}


gboolean block_land (void)
{
   int i, j;
   for (i = 0; i < block.num; i++)
   {
      if (cell_state_wo_block[block.x[i]][block.y[i] + 1]
       || block.y[i] + 1 >= Y_SIZE) {	/* cannot move down */
         for (j = 0; j < block.num; j++) {
            cell_state_wo_block[block.x[j]][block.y[j]] = block.color[j];
         }
         return FALSE;
      }
   }
  
   for (i = 0; i < block.num; i++) {
      cell_state[block.x[i]][block.y[i]] = 0;
   }
   for (i = 0; i < block.num; i++) {
      block.y[i]++;
      cell_state[block.x[i]][block.y[i]] = block.color[i];
   }
   return TRUE;
}


void game_over (GtkWidget * widget)
{
   GtkWidget *bonus_label;
#ifdef USE_GNOME
   gint pos;
#endif

   if (timeout) {
      g_source_remove (timeout);
   }
   if (level_timeout) {
      g_source_remove (level_timeout);
   }
   continue_game = 0;
   bonus_label = (GtkWidget *) g_object_get_data (G_OBJECT (main_window), label_name[0]);
   gtk_label_set_text (GTK_LABEL (bonus_label), _("Game over"));

///#ifdef USE_GNOME
///  if (sound_on)
///    gnome_triggers_do ("", NULL, "gtktetcolor", "gameover", NULL);
///  pos = gnome_score_log ((gfloat) score, NULL, TRUE);
///  if (pos <= 10 && pos >= 1)
///    gnome_scores_display (_("Gtktetcolor"), "gtktetcolor", NULL, pos);
///#endif
   insert_new_score ();
}


gboolean timeout_callback (gpointer window)
{
   GtkWidget *statusbar;
   GtkWidget *score_label_number, *bonus_label_number;
   GtkWidget *max_score_number_label, *bonus_label;
   gchar score_text[10], bonus_text[10], max_score_text[10];
   gchar *status_text, *status_score;
   gint i;

   score_label_number = (GtkWidget *) g_object_get_data (G_OBJECT (window), label_name[5]);
   max_score_number_label = (GtkWidget *) g_object_get_data (G_OBJECT (window), label_name[7]);
   if (block_land ()) {
      redraw_cells ();
   }
   else {
      g_source_remove (timeout);
      stop = 0;
      check_lines (window);
      redraw_cells ();
      max_score = max_score > score ? max_score : score;
      g_snprintf (score_text, 10, "%d", score);
      g_snprintf (max_score_text, 10, "%d", max_score);
      gtk_label_set_text (GTK_LABEL (score_label_number), score_text);
      gtk_label_set_text (GTK_LABEL (max_score_number_label), max_score_text);
      for (i = current_score_number - 1; i >= 0; i--) {
         if (score <= saved_score[i])
         break;
      }
      if (i + 1 != current_score_number)
      {
         GdkColormap *cmap;
         GdkColor text_color;
         GtkStyle *newstyle =
         gtk_style_copy (gtk_widget_get_style (score_label_number));
         current_score_number = i + 1;

         /*      cmap = gdk_colormap_get_system (); */
         cmap = gdk_drawable_get_colormap (score_label_number->window);
         switch (current_score_number)
         {
            case 9:
               text_color.red = 0x7fff;
               text_color.green = 0;
               text_color.blue = 0xffff;
               break;
            case 8:
               text_color.red = 0;
               text_color.green = 0;
               text_color.blue = 0xffff;
               break;
            case 7:
               text_color.red = 0;
               text_color.green = 0x7fff;
               text_color.blue = 0xffff;
               break;
            case 6:
               text_color.red = 0;
               text_color.green = 0xffff;
               text_color.blue = 0xffff;
               break;
            case 5:
               text_color.red = 0;
               text_color.green = 0xffff;
               text_color.blue = 0x7fff;
               break;
            case 4:
               text_color.red = 0;
               text_color.green = 0xffff;
               text_color.blue = 0;
               break;
            case 3:
               text_color.red = 0x7fff;
               text_color.green = 0xffff;
               text_color.blue = 0;
               break;
            case 2:
               text_color.red = 0xffff;
               text_color.green = 0xffff;
               text_color.blue = 0;
               break;
            case 1:
               text_color.red = 0xffff;
               text_color.green = 0x7fff;
               text_color.blue = 0;
               break;
            case 0:
               text_color.red = 0xffff;
               text_color.green = 0;
               text_color.blue = 0;
               break;
         }
         if (!gdk_colormap_alloc_color (cmap, &text_color, TRUE, TRUE)) {
            g_warning ("couldn't allocate color");
            goto nocolor;
         }
         newstyle->fg[0] = text_color;
         gtk_widget_set_style (score_label_number, newstyle);
         gdk_colormap_free_colors (cmap, &text_color, 1);
      }

   nocolor:
      statusbar = (GtkWidget *) g_object_get_data (G_OBJECT (window), "statusbar1");
      gtk_statusbar_pop (GTK_STATUSBAR (statusbar), 1);
      status_score = g_strdup (_("Score"));
      status_text = g_malloc (strlen (status_score) + 1 + 10 + 1);
      strcpy (status_text, status_score);
      strcat (status_text, " ");
      strcat (status_text, score_text);
      gtk_statusbar_push (GTK_STATUSBAR (statusbar), 1, status_text);
      g_free (status_score);
      g_free (status_text);
      if (bonus) {
///#ifdef USE_GNOME
///      if (sound_on)
///	gnome_triggers_do ("", NULL, "gtktetcolor", "bonus", NULL);
///#endif
         bonus_tic = 0;
         g_snprintf (bonus_text, 10, "%d", bonus);
         bonus_label_number = (GtkWidget *) g_object_get_data (G_OBJECT (window), "bonus_label_number");
         bonus_label = (GtkWidget *) g_object_get_data (G_OBJECT (window), "bonus_label");
         gtk_label_set_text (GTK_LABEL (bonus_label_number), bonus_text);
         gtk_label_set_text (GTK_LABEL (bonus_label), _("Bonus"));
      }
///#ifdef USE_GNOME
///    else if (sound_on && lines == 1)
///      gnome_triggers_do ("", NULL, "gtktetcolor", "linerem", NULL);
///#endif
      new_block ();
      redraw_cells ();
      return FALSE; 
   }

   return (TRUE);
}


gboolean change_level (gpointer window)
{
   gchar num[2], timer_buf[6];
   GtkWidget *label;
   GtkWidget *bonus_label_number, *bonus_label;
   time_t temp;
   struct tm *game_time;

   if (!(++level_tic % 60) && level < 9) {
      label = (GtkWidget *) g_object_get_data (G_OBJECT (window), "label_level_number");
      level++;
      g_snprintf (num, 2, "%d", level);
      gtk_label_set_text (GTK_LABEL (label), num);
   }
   if (++bonus_tic >= 5) {
       bonus_label_number = (GtkWidget *) g_object_get_data (G_OBJECT (window), "bonus_label_number");
       bonus_label = (GtkWidget *) g_object_get_data (G_OBJECT (window), "bonus_label");
       gtk_label_set_text (GTK_LABEL (bonus_label_number), "");
       gtk_label_set_text (GTK_LABEL (bonus_label), "");
       bonus_tic = 0;
   }
   label = (GtkWidget *) g_object_get_data (G_OBJECT (window), label_name[8]);
   temp = (time_t) level_tic;
   game_time = gmtime (&temp);
   g_snprintf (timer_buf, 6, "%d%c%02d", game_time->tm_min, ':', game_time->tm_sec);
   gtk_label_set_text (GTK_LABEL (label), timer_buf);
   return (TRUE);
}


gboolean shift_block (gint direction)
{
   gint newx, i;
   for (i = 0; i < block.num; i++) {
      newx = block.x[i] + direction;
      if (cell_state_wo_block[newx][block.y[i]] || newx >= X_SIZE || newx < 0)
         return FALSE;
   }
   for (i = 0; i < block.num; i++) {
      cell_state[block.x[i]][block.y[i]] = 0;
   }
   for (i = 0; i < block.num; i++) {
      block.x[i] += direction;
      cell_state[block.x[i]][block.y[i]] = block.color[i];
   }
   return TRUE;
}


gboolean rotate_block (gboolean (*rotate_cell) (gint i, gint * newx, gint * newy))
{
   gint newx1, newy1, newx2, newy2;
   switch (block.type)
   {
      case 1:
         if (!(*rotate_cell) (0, &newx1, &newy1))
            return FALSE;
         cell_state[block.x[0]][block.y[0]] = 0;
         change_rotated_cell (0, newx1, newy1);
         break;
      case 2:
      case 3:
         if (!(*rotate_cell) (0, &newx1, &newy1)) {
            return FALSE;
         }
         if (!(*rotate_cell) (2, &newx2, &newy2)) {
            return FALSE;
         }
         cell_state[block.x[0]][block.y[0]] = 0;
         cell_state[block.x[2]][block.y[2]] = 0;
         change_rotated_cell (0, newx1, newy1);
         change_rotated_cell (2, newx2, newy2);
         break;
   }
   return TRUE;
}


gboolean rotate_cell_right (gint i, gint * newx, gint * newy)
{
   *newx = block.y[1] - block.y[i] + block.x[1];
   *newy = block.x[i] - block.x[1] + block.y[1];
   if (cell_state_wo_block[*newx][*newy] || *newx < 0 || *newx >= X_SIZE
     || *newy >= Y_SIZE) {
      return FALSE;
   }
   return TRUE;
}


gboolean rotate_cell_left (gint i, gint * newx, gint * newy)
{
   *newx = block.y[i] - block.y[1] + block.x[1];
   *newy = block.x[1] - block.x[i] + block.y[1];
   if (cell_state_wo_block[*newx][*newy] || *newx < 0 || *newx >= X_SIZE
     || *newy >= Y_SIZE) {
     return FALSE;
   }
   return TRUE;
}


void change_rotated_cell (gint i, gint newx, gint newy)
{
   block.x[i] = newx;
   block.y[i] = newy;
   cell_state[block.x[i]][block.y[i]] = block.color[i];
}

gboolean seek_removing_cells (void)
{
   gint i, j, min_y_cell, add_score;
   gboolean return_val = FALSE;

   add_score = MIN_SCORE + MIN_SCORE * (level - 1) / 10;

   /* search for vertical lines */
   for (i = 0; i < X_SIZE; i++)
   {
      min_y_cell = Y_SIZE - 1;
      j = -1;
      do
         j++;
      while (!cell_state[i][j]);
      min_y_cell = j;
      for (j = min_y_cell; j < Y_SIZE - 2; j++)
      {
         if (cell_state[i][j] == cell_state[i][j + 1]
           && cell_state[i][j] == cell_state[i][j + 2] && cell_state[i][j])
         {
            removing_cell[i][j] = 1;
            removing_cell[i][j + 1] = 1;
            removing_cell[i][j + 2] = 1;
            if (j) {		/* check for 4 cells in line */
               if (cell_state[i][j] == cell_state[i][j - 1]) {
                  if (j > 1) {	/* check for 5 cells in line */
                     if (cell_state[i][j] == cell_state[i][j - 2]) {
                        add_score = add_score * (1.50 / 1.25);
                     } else {
                        add_score = add_score + add_score / 4;
                     }
                  } else {
                    add_score = add_score + add_score / 4;
                  }
               }
            }
            if (j == Y_SIZE - 3 || cell_state[i][j] != cell_state[i][j + 3]) {
               score += add_score;
              lines++;
            }
            return_val = TRUE;
         }
      }
   }

   /* search for horizontal lines */
   for (j = 0; j < Y_SIZE; j++)
   {
      for (i = 0; i < X_SIZE - 2; i++)
      {
         if (cell_state[i][j] && cell_state[i][j] == cell_state[i + 1][j]
           && cell_state[i][j] == cell_state[i + 2][j])
         {
            removing_cell[i][j] = 1;
            removing_cell[i + 1][j] = 1;
            removing_cell[i + 2][j] = 1;
            if (i) { /* check for 4 cells in line */
               if (cell_state[i][j] == cell_state[i - 1][j]) {
                  if (i > 1) { /* check for 5 cells in line */
                     if (cell_state[i][j] == cell_state[i - 2][j]) {
                        add_score = add_score * (1.50 / 1.25);
                     } else {
                        add_score = add_score + add_score / 4;
                     }
                  } else {
                     add_score = add_score + add_score / 4;
                  }
               }
            }
            if (i == X_SIZE - 3 || cell_state[i][j] != cell_state[i + 3][j]) {
               score += add_score;
               lines++;
            }
            return_val = TRUE;
         }
      }
   }

   /* search for diagonal lines */
   for (j = 2; j < Y_SIZE; j++)
   {
      for (i = 0; i < X_SIZE - 2; i++)
      {
         if (cell_state[i][j] && cell_state[i][j] == cell_state[i + 1][j - 1]
           && cell_state[i][j] == cell_state[i + 2][j - 2])
         {
            removing_cell[i][j] = 1;
            removing_cell[i + 1][j - 1] = 1;
            removing_cell[i + 2][j - 2] = 1;
            if (i && j < Y_SIZE - 1) {	/* check for 4 cells in line */
               if (cell_state[i][j] == cell_state[i - 1][j + 1]) {
                  if (i > 1 && j < Y_SIZE - 2) {	/* check for 5 cells in line */
                     if (cell_state[i][j] == cell_state[i - 2][j + 2]) {
                        add_score = add_score * (1.50 / 1.25);
                     } else {
                        add_score = add_score + add_score / 4;
                     }
                  } else {
                     add_score = add_score + add_score / 4;
                  }
               }
            }
            if ((i == X_SIZE - 3 && j == 2) || cell_state[i][j] != cell_state[i + 3][j - 3]) {
               score += add_score;
               lines++;
            }
            return_val = TRUE;
         }
      }
   }

   /* search for backward diagonal lines */
   for (j = 0; j < Y_SIZE - 2; j++)
   {
      for (i = 0; i < X_SIZE - 2; i++)
      {
         if (cell_state[i][j] && cell_state[i][j] == cell_state[i + 1][j + 1]
           && cell_state[i][j] == cell_state[i + 2][j + 2])
         {
            removing_cell[i][j] = 1;
            removing_cell[i + 1][j + 1] = 1;
            removing_cell[i + 2][j + 2] = 1;
            if (i && j) {		/* check for 4 cells in line */
               if (cell_state[i][j] == cell_state[i - 1][j - 1]) {
                  if (i > 1 && j > 1) {	/* check for 5 cells in line */
                     if (cell_state[i][j] == cell_state[i - 2][j - 2]) {
                        add_score = add_score * (1.50 / 1.25);
                     } else {
                        add_score = add_score + add_score / 4;
                     }
                  } else {
                     add_score = add_score + add_score / 4;
                  }
               }
            }
            if ((i == X_SIZE - 3 && j == Y_SIZE - 3) || cell_state[i][j] != cell_state[i + 3][j + 3]) {
               score += add_score;
               lines++;
            }
            return_val = TRUE;
         }
      }
   }
   return return_val;
}


void check_lines (GtkWidget * widget)
{
   gint i, j;
   gulong delay_time = destroy_delay * 1000;

   lines = 0;
   bonus = 0;
   for (j = 0; j < Y_SIZE; j++) {
      for (i = 0; i < X_SIZE; i++) {
         removing_cell[i][j] = 0;
      }
   }
   while (seek_removing_cells ())
   {
      for (j = 0; j < Y_SIZE; j++) {
         for (i = 0; i < X_SIZE; i++) {
            if (removing_cell[i][j]) {
               cell_state[i][j] = NUMBER_COLORS;
            }
         }
      }
      redraw_cells ();		/* Why it does not want to work first time ? */
      redraw_all_cells ();
      g_usleep (delay_time);
      remove_cells ();
   }
   if (lines > 1) {
      if (lines <= 11) {
         bonus = (lines - 1) * 500;
      } else if (lines <= 17) {
         bonus = 5000 + 2500 * (lines - 11);
      } else {
         bonus = 20000 + 5000 * (lines - 17);
         score += bonus;
      }
   }
}


gboolean remove_cells (void)
{
   gint i, j, k, prev;
   for (i = 0; i < X_SIZE; i++)
   {
      for (j = Y_SIZE - 1; j >= 0; j--)
      {
         if (removing_cell[i][j]) {
            k = j;
            j++;
            while (k >= 0) {
               removing_cell[i][k] = k ? removing_cell[i][k - 1] : 0;
               prev = k ? cell_state[i][k - 1] : 0;
               cell_state[i][k] = prev;
               cell_state_wo_block[i][k] = prev;
               if (!prev) {
                  break;
               }
               k--;
            }
         }
      }
   }
   return TRUE;
}
