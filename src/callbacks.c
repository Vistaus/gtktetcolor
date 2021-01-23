/* GtkTetcolor
 * Copyright (C) 1999 Andrey V. Panov
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

#include <gtk/gtk.h>
#include <glib/gi18n.h>
#include <stdlib.h>
#include <gdk/gdkkeysyms.h>
#include <stdio.h>
#include <string.h>

#include "callbacks.h"
#include "interface.h"
#include "main.h"
#include "score.h"
#include "game.h"
#include "preferences.h"
#include "pixmaps.h"

gint cell_width, left_margin, initial_level, use_graykeys;
///#ifdef USE_GNOME
gboolean sound_on;
///#else
gboolean text_toolbar;
///#endif
gint nav_keys[4], alt_nav_keys[4];
GtkStyle *label_style;
GdkFont *font;
gchar *font_name;
gchar *label_name[MAX_LABEL];	/* Label identifiers */

gboolean preferences_changed;


gboolean
on_main_window_delete_event (GtkWidget * widget,
			     GdkEvent * event, gpointer user_data)
{
  gtk_main_quit ();
  return FALSE;
}


void
on_new_activate (GtkWidget * menuitem, gpointer user_data)
{
  init_game (menuitem);
}


void
on_scores_activate (GtkWidget * menuitem, gpointer user_data)
{
  create_scores_dialog ();
}


void
on_quit_activate (GtkWidget * menuitem, gpointer user_data)
{
  gtk_main_quit ();
  gtk_widget_destroy (main_window);
}


gboolean
on_main_window_key_press_event (GtkWidget * widget,
				GdkEventKey * event, gpointer user_data)
{
  gint key = event->keyval;

  if (!timeout || !continue_game)
    return FALSE;
  if (key == nav_keys[0] || key == alt_nav_keys[0]) {
    if (shift_block (1))
      redraw_cells ();
    return TRUE;
  }
  else if (key == nav_keys[1] || key == alt_nav_keys[1]) {
    if (shift_block (-1))
      redraw_cells ();
    return TRUE;
  }
  else if (key == nav_keys[2] || key == alt_nav_keys[2]) {
    if (block.type)
      if (rotate_block (rotate_cell_left))
	redraw_cells ();
    return TRUE;
  }
  else if (key == nav_keys[3] || key == alt_nav_keys[3]) {
    if (block.type)
      if (rotate_block (rotate_cell_right))
	redraw_cells ();
    return TRUE;
  }
  else if (key == GDK_space) {
    if (timeout) {
      g_source_remove (timeout);
      interval = INI_INTERVAL / 100;
      timeout = g_timeout_add (interval, (GSourceFunc) timeout_callback, main_window);
      return TRUE;
    }
    else
      return FALSE;
  }
  return FALSE;
}


void
on_name_response (GtkObject * obj, gpointer user_data)
{
  GtkWidget *widget;
  gchar *str;

  widget = (GtkWidget *) g_object_get_data (G_OBJECT (obj), "name_entry");
  str = g_strdup (gtk_entry_get_text (GTK_ENTRY (widget)));
  if (strlen (str) == 0) {
    str = g_strdup (g_get_real_name ());
    if (strlen (str) == 0)
      str = g_strdup (g_get_user_name ());
  }
  if (strlen(str) > 255){
    strncpy (new_name, str, 255);
    new_name[255] = '\0';
  }
  else
    strcpy (new_name, str);
  if (str)
    g_free (str);

  strcpy (name[name_i], new_name);
  saved_score[name_i] = score;
  write_score ();
  gtk_widget_destroy ((GtkWidget *) obj);
}


void
on_pause_response (GtkObject * param, gpointer user_data)
{
  GtkWidget *pause_dialog;

  continue_game = 1;
  pause_dialog = (GtkWidget *) param;
  interval = INI_INTERVAL - INI_INTERVAL * (level - 1) / 10;
  timeout = g_timeout_add (interval, (GSourceFunc) timeout_callback, main_window);
  level_timeout = g_timeout_add (LEVEL_INT, (GSourceFunc) change_level, main_window);
  gtk_widget_destroy (pause_dialog);
}

void
on_pause_activate (GtkWidget * menuitem, gpointer user_data)
{
  if (continue_game) {
    create_pause_dialog ();
    if (timeout)
      g_source_remove (timeout);
    if (level_timeout)
      g_source_remove (level_timeout);
    continue_game = 0;
  }
}


void
help_on_keys_activate (GtkWidget * menuitem, gpointer user_data)
{
  create_help_dialog ();
}


/*
gboolean
on_main_window_expose_event (GtkWidget * widget,
			     GdkEventExpose * event, gpointer user_data)
{
  redraw_all_cells ();
  return FALSE;
}
*/

void
on_preferences_activate (GtkWidget * menuitem, gpointer user_data)
{
  create_preferences_dialog ();
}


void
preferences_dialog_response (GtkDialog * dialog, gint response_id,
			     gpointer data)
{
  switch (response_id) {
  case GTK_RESPONSE_OK:	/* OK */
    change_preferences (GTK_WIDGET (dialog));
    if (preferences_changed)
      save_preferences ();
    if (dialog)
      gtk_widget_destroy (GTK_WIDGET (dialog));
    break;
  case GTK_RESPONSE_APPLY:	/* APPLY */
    change_preferences (GTK_WIDGET (dialog));
    if (preferences_changed)
      save_preferences ();
    break;
  case GTK_RESPONSE_CLOSE:	/* CLOSE */
  case GTK_RESPONSE_DELETE_EVENT:
    if (dialog)
      gtk_widget_destroy (GTK_WIDGET (dialog));
    break;
  }
}


gboolean
on_drawingarea_expose_event (GtkWidget * widget,
			     GdkEventExpose * event, gpointer user_data)
{
  redraw_all_cells ();
  return FALSE;
}

void
on_numkeys_radiobutton_clicked (GtkButton * button, gpointer user_data)
{
  use_graykeys = 0;
  preferences_changed = TRUE;
}


void
on_graykeys_radiobutton_clicked (GtkButton * button, gpointer user_data)
{
  use_graykeys = 1;
  preferences_changed = TRUE;
}

///#ifdef USE_GNOME
void
on_sound_checkbutton_toggled (GtkToggleButton * togglebutton,
			      gpointer user_data)
{
  sound_on = !sound_on;
  preferences_changed = TRUE;
}
///#else
void
on_text_toolbar_checkbutton_toggled (GtkToggleButton * togglebutton,
				     gpointer user_data)
{
  GtkWidget *toolbar;

  text_toolbar = !text_toolbar;
  preferences_changed = TRUE;
  toolbar = (GtkWidget *) g_object_get_data (G_OBJECT (main_window), "toolbar");
  if (text_toolbar)
    gtk_toolbar_set_style (GTK_TOOLBAR (toolbar), GTK_TOOLBAR_BOTH);
  else
    gtk_toolbar_set_style (GTK_TOOLBAR (toolbar), GTK_TOOLBAR_ICONS);
}
///#endif
