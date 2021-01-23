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

#include <gtk/gtk.h>


gboolean
on_main_window_delete_event (GtkWidget * widget,
			     GdkEvent * event, gpointer user_data);

void on_new_activate (GtkWidget * menuitem, gpointer user_data);

void on_scores_activate (GtkWidget * menuitem, gpointer user_data);

void on_quit_activate (GtkWidget * menuitem, gpointer user_data);

void on_about_activate (GtkWidget * menuitem, gpointer user_data);

void on_pause_activate (GtkWidget * menuitem, gpointer user_data);

void help_on_keys_activate (GtkWidget * menuitem, gpointer user_data);

void on_preferences_activate (GtkWidget * menuitem, gpointer user_data);

gboolean
on_main_window_key_press_event (GtkWidget * widget,
				GdkEventKey * event, gpointer user_data);

void on_name_response (GtkObject * obj, gpointer user_data);

void on_pause_response (GtkObject * param, gpointer user_data);

gboolean
on_main_window_expose_event (GtkWidget * widget,
			     GdkEventExpose * event, gpointer user_data);

void
preferences_dialog_response (GtkDialog * dialog, gint response_id,
			     gpointer data);

gboolean
on_drawingarea_expose_event (GtkWidget * widget,
			     GdkEventExpose * event, gpointer user_data);

void on_numkeys_radiobutton_clicked (GtkButton * button, gpointer user_data);

void on_graykeys_radiobutton_clicked (GtkButton * button, gpointer user_data);

void on_font_ok_button_clicked (GtkButton * button, gpointer user_data);

void on_font_cancel_button_clicked (GtkButton * button, gpointer user_data);

void on_font_apply_button_clicked (GtkButton * button, gpointer user_data);

void on_fontsel_button_clicked (GtkButton * button, gpointer user_data);

void
on_sound_checkbutton_toggled (GtkToggleButton * togglebutton,
			      gpointer user_data);

void
on_text_toolbar_checkbutton_toggled (GtkToggleButton * togglebutton,
				     gpointer user_data);
