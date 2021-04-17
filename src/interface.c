/* GtkTetcolor
 * Copyright (C) 2000, 2001 Andrey V. Panov
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

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>
#include <glib/gi18n.h>

#include "callbacks.h"
#include "interface.h"
#include "main.h"
#include "score.h"
#include "preferences.h"

#include "../pixmaps/scores.xpm"
#include "../pixmaps/pause.xpm"

#include "interface_menu.c"

gint cell_width, initial_level, destroy_delay;
///#ifdef USE_GNOME
gboolean sound_on;
///#endif
GtkStyle *label_style;
gchar *font_name;
GdkPixbuf *icon_xpm;

gchar *label_name[MAX_LABEL] =
{
   "bonus_label",
   "bonus_label_number",
   "level_label",
   "label_level_number",
   "score_label",
   "score_label_number",
   "max_score_label",
   "max_score_number_label",
   "timer_label",
   "empty_label" /* for proper visualization of left margin vbox */
};


GtkWidget * create_main_window (void)
{
   GtkWidget *vbox1;
   GtkWidget *tmp_toolbar_icon;
   GtkAccelGroup *accel_group;
   GtkWidget *working_hbox;
   GtkWidget *left_margin_vbox;
   GtkWidget *bonus_label;
   GtkWidget *bonus_label_number;
   GtkWidget *level_label;
   GtkWidget *label_level_number;
   GtkWidget *score_label;
   GtkWidget *score_label_number;
   GtkWidget *empty_label;
   GtkWidget *right_margin_vbox;
   GtkWidget *max_score_label;
   GtkWidget *max_score_number_label;
   GtkWidget *timer_label;
   GtkWidget *statusbar1;

   accel_group = gtk_accel_group_new ();

   main_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

   gtk_window_set_title (GTK_WINDOW (main_window), _("gtktetcolor"));
   gtk_window_set_resizable (GTK_WINDOW (main_window), FALSE);

   vbox1 = gtk_vbox_new (FALSE, 0);
   gtk_container_add (GTK_CONTAINER (main_window), vbox1);

   GdkPixbuf * scores_pixbuf = gdk_pixbuf_new_from_xpm_data ((const char **) &scores_xpm);
   GdkPixbuf * pause_pixbuf = gdk_pixbuf_new_from_xpm_data ((const char **) &pause_xpm);

   create_menubar (GTK_BOX (vbox1), GTK_WINDOW (main_window));

   working_hbox = gtk_hbox_new (FALSE, 0);
   gtk_box_pack_start (GTK_BOX (vbox1), working_hbox, TRUE, TRUE, 0);

   left_margin_vbox = gtk_vbox_new (FALSE, 5);
   gtk_box_pack_start (GTK_BOX (working_hbox), left_margin_vbox, TRUE, TRUE, 0);
   gtk_container_set_border_width (GTK_CONTAINER (left_margin_vbox), 10);

   bonus_label = gtk_label_new ("");
   g_object_set_data_full (G_OBJECT (main_window), label_name[0], bonus_label, NULL);
   gtk_box_pack_start (GTK_BOX (left_margin_vbox), bonus_label, FALSE, FALSE, 0);

   bonus_label_number = gtk_label_new ("");
   g_object_set_data_full (G_OBJECT (main_window), label_name[1], bonus_label_number, NULL);
    gtk_box_pack_start (GTK_BOX (left_margin_vbox), bonus_label_number, FALSE, FALSE, 0);

   level_label = gtk_label_new (_("Level"));
   g_object_set_data_full (G_OBJECT (main_window), label_name[2], level_label, NULL);
   gtk_box_pack_start (GTK_BOX (left_margin_vbox), level_label, FALSE, FALSE, 0);

   label_level_number = gtk_label_new ("");
   g_object_set_data_full (G_OBJECT (main_window), label_name[3], label_level_number, NULL);
   gtk_box_pack_start (GTK_BOX (left_margin_vbox), label_level_number, FALSE, FALSE, 0);

   score_label = gtk_label_new (_("Score"));
   g_object_set_data_full (G_OBJECT (main_window), label_name[4], score_label, NULL);
   gtk_box_pack_start (GTK_BOX (left_margin_vbox), score_label, FALSE, FALSE, 0);

   score_label_number = gtk_label_new ("0");
   g_object_set_data_full (G_OBJECT (main_window), label_name[5], score_label_number, NULL);
   gtk_box_pack_start (GTK_BOX (left_margin_vbox), score_label_number, FALSE, FALSE, 0);

   empty_label = gtk_label_new ("");
   g_object_set_data_full (G_OBJECT (main_window), label_name[MAX_LABEL - 1], empty_label, NULL);
   gtk_box_pack_end (GTK_BOX (left_margin_vbox), empty_label, FALSE, FALSE, 0);

   drawingarea = gtk_drawing_area_new ();
   gtk_box_pack_start (GTK_BOX (working_hbox), drawingarea, TRUE, TRUE, 0);
   gtk_widget_set_size_request (drawingarea, X_SIZE * cell_width + 2 * border,
                                Y_SIZE * cell_width + 2 * border);

   right_margin_vbox = gtk_vbox_new (FALSE, 5);
   gtk_box_pack_start (GTK_BOX (working_hbox), right_margin_vbox, TRUE, TRUE, 0);
   gtk_container_set_border_width (GTK_CONTAINER (right_margin_vbox), 10);

   max_score_label = gtk_label_new (_("max score"));
   g_object_set_data_full (G_OBJECT (main_window), label_name[6], max_score_label, NULL);
   gtk_box_pack_start (GTK_BOX (right_margin_vbox), max_score_label, FALSE, FALSE, 0);

   max_score_number_label = gtk_label_new ("");
   g_object_set_data_full (G_OBJECT (main_window), label_name[7], max_score_number_label, NULL);
   gtk_box_pack_start (GTK_BOX (right_margin_vbox), max_score_number_label, FALSE, FALSE, 0);

   timer_label = gtk_label_new ("");
   g_object_set_data_full (G_OBJECT (main_window), label_name[8], timer_label, NULL);
   gtk_box_pack_end (GTK_BOX (right_margin_vbox), timer_label, FALSE, FALSE, 0);

   change_font_of_labels ();

   statusbar1 = gtk_statusbar_new ();
   g_object_set_data_full (G_OBJECT (main_window), "statusbar1", statusbar1, NULL);
   gtk_box_pack_start (GTK_BOX (vbox1), statusbar1, FALSE, TRUE, 0);

   g_signal_connect (G_OBJECT (main_window), "delete-event",
                     G_CALLBACK (on_main_window_delete_event), NULL);
   g_signal_connect (G_OBJECT (main_window), "key-press-event",
                     G_CALLBACK (on_main_window_key_press_event), NULL);
   g_signal_connect (G_OBJECT (drawingarea), "expose-event",
                     G_CALLBACK (on_drawingarea_expose_event), NULL);

   gtk_window_add_accel_group (GTK_WINDOW (main_window), accel_group);

   gtk_widget_grab_focus (drawingarea);
   gtk_widget_show_all (main_window);
   return main_window;
}


void create_about_dialog (void)
{
   GtkWidget * w;
   GdkPixbuf * logo;
   const gchar * authors[] =
   {
       "Andrey V. Panov <panov@canopus.iacp.dvo.ru>",
       "wdlkmpx (github)",
       NULL
   };
   /* TRANSLATORS: Replace this string with your names, one name per line. */
   gchar * translators = _("Translated by");

   logo = icon_xpm;

   w = g_object_new (GTK_TYPE_ABOUT_DIALOG,
                     "version",      VERSION,
                     "program-name", "gtktetcolor",
                     "copyright",    "(C) 1999-2021",
                     "comments",     _("GtkTetcolor is a reimplementation of tetcolor game made for DOS by S. Sotnikov in 1991"),
                     "license",      "GPL2",
                     "website",      "https://github.com/wdlkmpx/gtktetcolor",
                     "authors",      authors,
                     "logo",         logo,
                     "translator-credits", translators,
                     NULL);
   gtk_container_set_border_width (GTK_CONTAINER (w), 2);
   gtk_window_set_transient_for (GTK_WINDOW (w), GTK_WINDOW (main_window));
   gtk_window_set_modal (GTK_WINDOW (w), TRUE);
   gtk_window_set_position (GTK_WINDOW (w), GTK_WIN_POS_CENTER_ON_PARENT);

   g_signal_connect_swapped (w, "response", G_CALLBACK (gtk_widget_destroy), w);
   gtk_widget_show_all (GTK_WIDGET (w));
}



void
create_scores_dialog (void)
{
   GtkWidget *dialog;
   GtkWidget *scores_frame;
   GtkWidget *scores_table;
   GtkWidget *name_label[10], *score_label[10];
   gint i;
   gchar str[10];

   dialog = gtk_dialog_new_with_buttons (_("Top scores"),
                       GTK_WINDOW (main_window),
                       GTK_DIALOG_MODAL |
                       GTK_DIALOG_DESTROY_WITH_PARENT,
                       GTK_STOCK_OK, GTK_RESPONSE_NONE, NULL);
   gtk_window_set_resizable (GTK_WINDOW (dialog), FALSE);

   g_signal_connect_swapped (GTK_OBJECT (dialog), "response",
                            G_CALLBACK (gtk_widget_destroy),
                            GTK_OBJECT (dialog));

   scores_frame = gtk_frame_new (_("Hall of fame"));
   gtk_container_add (GTK_CONTAINER (GTK_DIALOG (dialog)->vbox), scores_frame);
   gtk_container_set_border_width (GTK_CONTAINER (scores_frame), 5);

   scores_table = gtk_table_new (10, 2, FALSE);
   gtk_container_add (GTK_CONTAINER (scores_frame), scores_table);
   gtk_container_set_border_width (GTK_CONTAINER (scores_table), 10);
   gtk_table_set_col_spacings (GTK_TABLE (scores_table), 20);
   gtk_table_set_row_spacings (GTK_TABLE (scores_table), 5);

   read_score ();
   for (i = 0; i < 10; i++) {
      name_label[i] = gtk_label_new (name[i]);
      g_snprintf (str, 10, "%d", saved_score[i]);
      score_label[i] = gtk_label_new (str);
      gtk_table_attach_defaults (GTK_TABLE (scores_table), name_label[i], 0, 1, i, i + 1);
      gtk_misc_set_alignment (GTK_MISC (name_label[i]), 0., GTK_MISC (name_label[i])->yalign);
      gtk_table_attach_defaults (GTK_TABLE (scores_table), score_label[i], 1, 2, i, i + 1);
      gtk_misc_set_alignment (GTK_MISC (score_label[i]), 1., GTK_MISC (score_label[i])->yalign);
   }

   gtk_widget_show_all (dialog);
}


void create_name_dialog (void)
{
   GtkWidget *dialog;
   GtkWidget *name_frame;
   GtkWidget *name_entry;

   dialog = gtk_dialog_new_with_buttons (_("Enter name"),
                       GTK_WINDOW (main_window),
                       GTK_DIALOG_MODAL |
                       GTK_DIALOG_DESTROY_WITH_PARENT,
                       GTK_STOCK_OK, GTK_RESPONSE_NONE, NULL);
   gtk_window_set_resizable (GTK_WINDOW (dialog), FALSE);

   g_signal_connect_swapped (GTK_OBJECT (dialog), "response",
                             G_CALLBACK (on_name_response),
                             GTK_OBJECT (dialog));

   name_frame = gtk_frame_new (_("Enter your name"));
   gtk_container_set_border_width (GTK_CONTAINER (name_frame), 5);
   gtk_container_add (GTK_CONTAINER (GTK_DIALOG (dialog)->vbox), name_frame);

   name_entry = gtk_entry_new ();
   gtk_entry_set_max_length (GTK_ENTRY (name_entry), 255);
   if (strlen (new_name) != 0) {
      gtk_entry_set_text (GTK_ENTRY (name_entry), new_name);
      gtk_editable_select_region (GTK_EDITABLE (name_entry), 0, strlen (new_name));
   }
   g_object_set_data_full (G_OBJECT (dialog), "name_entry", name_entry, NULL);
   gtk_container_add (GTK_CONTAINER (name_frame), name_entry);
   gtk_widget_grab_focus (name_entry);
   gtk_widget_show_all (dialog);
}


void
create_pause_dialog (void)
{
   GtkWidget *dialog;
   GtkWidget *label;

   dialog = gtk_dialog_new_with_buttons (_("Game is paused"),
                       GTK_WINDOW (main_window),
                       GTK_DIALOG_MODAL |
                       GTK_DIALOG_DESTROY_WITH_PARENT,
                       GTK_STOCK_OK, GTK_RESPONSE_NONE, NULL);
   gtk_window_set_resizable (GTK_WINDOW (dialog), FALSE);

   g_signal_connect_swapped (GTK_OBJECT (dialog), "response",
                             G_CALLBACK (on_pause_response),
                             GTK_OBJECT (dialog));

   label = gtk_label_new (_("Gtktetcolor is paused"));

   gtk_misc_set_padding (GTK_MISC (label), 10, 10);

   gtk_container_add (GTK_CONTAINER (GTK_DIALOG (dialog)->vbox), label);
   gtk_widget_show_all (dialog);
}


void
create_help_dialog (void)
{
   GtkWidget *dialog;
   GtkWidget *label;

   dialog = gtk_dialog_new_with_buttons (_("Help on keys"),
                       GTK_WINDOW (main_window),
                       GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                       GTK_STOCK_OK, GTK_RESPONSE_NONE, NULL);
   gtk_window_set_resizable (GTK_WINDOW (dialog), FALSE);

   g_signal_connect_swapped (GTK_OBJECT (dialog), "response",
                             G_CALLBACK (gtk_widget_destroy),
                             GTK_OBJECT (dialog));

   label = gtk_label_new (_(
                       "Left arrow  / Num 4 - shift block left\n"
                       "Down arrow  / Num 5 - rotate block clockwise\n"
                       "Right arrow / Num 6 - shift block right\n"
                       "Up arrow    / Num 8 - rotate block counter-clockwise\n"
                       "Space - drop block"));

   gtk_misc_set_padding (GTK_MISC (label), 10, 10);

   gtk_container_add (GTK_CONTAINER (GTK_DIALOG (dialog)->vbox), label);
   gtk_widget_show_all (dialog);
}


void create_preferences_dialog (void)
{
   GtkWidget *preferences_dialog;
   GtkWidget *preferences_vbox;
   GtkWidget *preferences_hbox1;
   GtkWidget *cell_size_label;
   GtkObject *cell_size_spinbutton_adj;
   GtkWidget *cell_size_spinbutton;
   GtkWidget *preferences_hbox2;
   GtkWidget *initial_level_label;
   GtkObject *initial_level_spinbutton_adj;
   GtkWidget *initial_level_spinbutton;
   GtkWidget *preferences_destroy_delay_hbox;
   GtkWidget *destroy_delay_label;
   GtkObject *destroy_delay_spinbutton_adj;
   GtkWidget *destroy_delay_spinbutton;
   GtkWidget *preferences_hbox3;
   GtkWidget *prefs_notebook;
   GtkWidget *options_label;
   GtkWidget *fontselection;
   GtkWidget *fonts_label;

///#ifdef USE_GNOME
   GtkWidget *sound_hbox;
   GtkWidget *sound_checkbutton;
///#endif

   preferences_dialog = gtk_dialog_new_with_buttons (_("GtkTetcolor preferences"),
                            GTK_WINDOW (main_window),
                            GTK_DIALOG_DESTROY_WITH_PARENT,
                            GTK_STOCK_OK, GTK_RESPONSE_OK,
                            GTK_STOCK_APPLY, GTK_RESPONSE_APPLY,
                            GTK_STOCK_CLOSE, GTK_RESPONSE_CLOSE, NULL);
   g_signal_connect_swapped (GTK_OBJECT (preferences_dialog), "response",
                             G_CALLBACK (preferences_dialog_response),
                             GTK_OBJECT (preferences_dialog));

   prefs_notebook = gtk_notebook_new ();
   gtk_box_pack_start (GTK_BOX (GTK_DIALOG (preferences_dialog)->vbox), prefs_notebook, FALSE, TRUE, 10);

   preferences_vbox = gtk_vbox_new (FALSE, 5);
   options_label = gtk_label_new (_("Game options"));

   gtk_container_add (GTK_CONTAINER (prefs_notebook), preferences_vbox);
   gtk_notebook_set_tab_label (GTK_NOTEBOOK (prefs_notebook),
                               gtk_notebook_get_nth_page (GTK_NOTEBOOK (prefs_notebook), 0),
                               options_label);

   fontselection = gtk_font_selection_new ();
   g_object_set_data_full (G_OBJECT (preferences_dialog), "fontselection", fontselection, NULL);

   fonts_label = gtk_label_new (_("Font for labels"));
   gtk_container_add (GTK_CONTAINER (prefs_notebook), fontselection);
   gtk_notebook_set_tab_label (GTK_NOTEBOOK (prefs_notebook),
                               gtk_notebook_get_nth_page (GTK_NOTEBOOK (prefs_notebook), 1),
                               fonts_label);

   preferences_hbox1 = gtk_hbox_new (FALSE, 0);
   gtk_box_pack_start (GTK_BOX (preferences_vbox), preferences_hbox1, FALSE, TRUE, 10);

   cell_size_label = gtk_label_new (_("Cell size: "));
   gtk_box_pack_start (GTK_BOX (preferences_hbox1), cell_size_label, FALSE, FALSE, 10);

   cell_size_spinbutton_adj = gtk_adjustment_new ((gfloat) cell_width, 10, MAX_CELL_SIZE, 1, 10, 10);
   cell_size_spinbutton = gtk_spin_button_new (GTK_ADJUSTMENT (cell_size_spinbutton_adj), 1, 0);
   g_object_set_data_full (G_OBJECT (preferences_dialog), "cell_size_spinbutton", cell_size_spinbutton, NULL);

   gtk_box_pack_start (GTK_BOX (preferences_hbox1), cell_size_spinbutton, FALSE, TRUE, 10);

   preferences_hbox2 = gtk_hbox_new (FALSE, 0);
   gtk_box_pack_start (GTK_BOX (preferences_vbox), preferences_hbox2, FALSE, TRUE, 10);

   initial_level_label = gtk_label_new (_("Initial level: "));
   gtk_box_pack_start (GTK_BOX (preferences_hbox2), initial_level_label, FALSE, FALSE, 10);

   initial_level_spinbutton_adj = gtk_adjustment_new ((gfloat) initial_level, 1, 9, 1, 10, 10);
   initial_level_spinbutton = gtk_spin_button_new (GTK_ADJUSTMENT (initial_level_spinbutton_adj), 1, 0);
   g_object_set_data_full (G_OBJECT (preferences_dialog), "initial_level_spinbutton", initial_level_spinbutton, NULL);
   gtk_box_pack_start (GTK_BOX (preferences_hbox2), initial_level_spinbutton, FALSE, TRUE, 10);

   preferences_destroy_delay_hbox = gtk_hbox_new (FALSE, 0);
   gtk_box_pack_start (GTK_BOX (preferences_vbox), preferences_destroy_delay_hbox, FALSE, TRUE, 10);

   destroy_delay_label = gtk_label_new (_("Delay before cell destroying, ms: "));
   gtk_box_pack_start (GTK_BOX (preferences_destroy_delay_hbox), destroy_delay_label, FALSE, FALSE, 10);

   destroy_delay_spinbutton_adj = gtk_adjustment_new ((gfloat) destroy_delay, 5, MAX_DESTROY_DELAY, 5, 10, 10);
   destroy_delay_spinbutton = gtk_spin_button_new (GTK_ADJUSTMENT (destroy_delay_spinbutton_adj), 1, 0);
   g_object_set_data_full (G_OBJECT (preferences_dialog), "destroy_delay_spinbutton", destroy_delay_spinbutton, NULL);
   gtk_box_pack_start (GTK_BOX (preferences_destroy_delay_hbox), destroy_delay_spinbutton, FALSE, TRUE, 10);

   preferences_hbox3 = gtk_hbox_new (FALSE, 0);
   gtk_box_pack_start (GTK_BOX (preferences_vbox), preferences_hbox3, FALSE, TRUE, 10);

///#ifdef USE_GNOME
   sound_hbox = gtk_hbox_new (FALSE, 0);
   gtk_box_pack_start (GTK_BOX (preferences_vbox), sound_hbox, FALSE, FALSE, 10);

   sound_checkbutton = gtk_check_button_new_with_label (_("Use sound"));
   g_object_set_data_full (G_OBJECT (preferences_dialog), "sound_checkbutton", sound_checkbutton, NULL);
   gtk_box_pack_start (GTK_BOX (sound_hbox), sound_checkbutton, FALSE, FALSE, 10);

   if (sound_on)
      gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (sound_checkbutton), TRUE);
   else
      gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (sound_checkbutton), FALSE);

   g_signal_connect (G_OBJECT (sound_checkbutton), "toggled",
                     G_CALLBACK (on_sound_checkbutton_toggled), NULL);
///#endif

   gtk_widget_show_all (preferences_dialog);
  
   if (font_name)
      gtk_font_selection_set_font_name (GTK_FONT_SELECTION (fontselection), font_name);
}
