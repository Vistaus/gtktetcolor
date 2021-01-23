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

#ifdef USE_GNOME
#include <gnome.h>
#endif

#include "callbacks.h"
#include "interface.h"
#include "main.h"
#include "score.h"
#include "preferences.h"

#ifndef USE_GNOME
#include "../pixmaps/scores.xpm"
#include "../pixmaps/pause.xpm"
#endif

gint cell_width, initial_level, use_graykeys, destroy_delay;
#ifdef USE_GNOME
gboolean sound_on;
#else
gboolean text_toolbar;
#endif
GtkStyle *label_style;
gchar *font_name;
GdkPixbuf *icon_xpm;

gchar *label_name[MAX_LABEL] = { "bonus_label",
  "bonus_label_number",
  "level_label",
  "label_level_number",
  "score_label",
  "score_label_number",
  "max_score_label",
  "max_score_number_label",
  "timer_label",
  "empty_label"			/* for proper visualization of left margin vbox */
};

#ifdef USE_GNOME
static GnomeUIInfo Game_menu_uiinfo[] = {
  GNOMEUIINFO_MENU_NEW_GAME_ITEM (on_new_activate, NULL),
  {
   GNOME_APP_UI_ITEM, N_("_Pause"),
   N_("Pause game"),
   on_pause_activate, NULL, NULL,
   GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_TIMER_STOP,
   GDK_P, 0, NULL},
  GNOMEUIINFO_MENU_SCORES_ITEM (on_scores_activate, NULL),
  GNOMEUIINFO_MENU_PREFERENCES_ITEM (on_preferences_activate, NULL),
  GNOMEUIINFO_MENU_EXIT_ITEM (on_quit_activate, NULL),
  GNOMEUIINFO_END
};

static GnomeUIInfo Help_menu_uiinfo[] = {
  {
   GNOME_APP_UI_ITEM, N_("on keys"),
   N_("Help on keys"),
   help_on_keys_activate, NULL, NULL,
   GNOME_APP_PIXMAP_NONE, NULL,
   0, 0, NULL},
  GNOMEUIINFO_MENU_ABOUT_ITEM (create_about_dialog, NULL),
  GNOMEUIINFO_END
};

static GnomeUIInfo menubar1_uiinfo[] = {
  GNOMEUIINFO_MENU_GAME_TREE (Game_menu_uiinfo),
  GNOMEUIINFO_MENU_HELP_TREE (Help_menu_uiinfo),
  GNOMEUIINFO_END
};

static GnomeUIInfo toolbar[] = {
  GNOMEUIINFO_ITEM_STOCK (N_("New"), N_("New game"), on_new_activate,
			  GTK_STOCK_NEW),
  GNOMEUIINFO_ITEM_STOCK (N_("Scores"), N_("Top scores"),
			  on_scores_activate,
			  GNOME_STOCK_SCORES),
  GNOMEUIINFO_ITEM_STOCK (N_("Pause"), N_("Pause game"),
			  on_pause_activate,
			  GNOME_STOCK_TIMER_STOP),
  GNOMEUIINFO_ITEM_STOCK (N_("Props"), N_("Preferences"),
			  on_preferences_activate,
			  GTK_STOCK_PROPERTIES),
  GNOMEUIINFO_ITEM_STOCK (N_("Quit"), N_("Quit game"), on_quit_activate,
			  GTK_STOCK_QUIT),

  GNOMEUIINFO_END
};
#else
# ifdef ITEMFACTORY
static GtkItemFactoryEntry menu_items[] = {
  {N_("/_Game"), NULL, 0, 0, "<Branch>"},
  {N_("/Game/_New"), NULL, on_new_activate, 0, "<StockItem>", GTK_STOCK_NEW},
  {N_("/Game/_Pause"), "P", on_pause_activate, 0, "<Item>", NULL},
  {N_("/Game/_Scores"), NULL, on_scores_activate, 0, "<Item>", NULL},
  {N_("/Game/_Preferences"), NULL, on_preferences_activate, 0, "<StockItem>",
   GTK_STOCK_PROPERTIES},
  {N_("/Game/_Quit"), NULL, on_quit_activate, 0, "<StockItem>", GTK_STOCK_QUIT},
  {N_("/_Help"), NULL, 0, 0, "<Branch>", GTK_STOCK_HELP},
  {N_("/Help/on keys"), NULL, help_on_keys_activate, 0, "<Item>", NULL},
  {N_("/Help/_About"), NULL, create_about_dialog, 0, "<Item>", NULL}
};
# endif
#endif

GtkWidget *
create_main_window (void)
{
#ifndef USE_GNOME
  GtkWidget *vbox1;
  GtkWidget *menubar1;
  GtkWidget *Game;
  GtkWidget *Game_menu;
  GtkWidget *new;
  GtkWidget *pause;
  GtkWidget *scores;
  GtkWidget *preferences;
  GtkWidget *quit;
  GtkWidget *Help;
  GtkWidget *Help_menu;
  GtkWidget *on_keys;
  GtkWidget *about;
  GtkWidget *toolbar1;
  GtkWidget *tmp_toolbar_icon;
/*
  GtkWidget *new_button;
  GtkWidget *scores_button;
  GtkWidget *pause_button;
  GtkWidget *preferences_button;
  GtkWidget *exit_button;
 */
  GtkToolItem *new_button;
  GtkToolItem *scores_button;
  GtkToolItem *pause_button;
  GtkToolItem *preferences_button;
  GtkToolItem *quit_button;
  GtkTooltips *tooltips;
  GtkAccelGroup *accel_group;
  GtkIconFactory *gtktetcolor_factory;
  GtkIconSet *tmp_iconset;
  GdkPixbuf *tmp_pixbuf;
# ifdef ITEMFACTORY
  GtkItemFactory *item_factory;
# endif
#endif
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

#ifdef USE_GNOME
  main_window = gnome_app_new (PACKAGE, _("gtktetcolor"));
#else
  accel_group = gtk_accel_group_new ();

  main_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
#endif
  gtk_window_set_title (GTK_WINDOW (main_window), _("gtktetcolor"));
  gtk_window_set_resizable (GTK_WINDOW (main_window), FALSE);

#ifndef USE_GNOME
  vbox1 = gtk_vbox_new (FALSE, 0);
  gtk_widget_set_name (vbox1, "vbox1");
  g_object_ref (G_OBJECT (vbox1));
  g_object_set_data_full (G_OBJECT (main_window), "vbox1", vbox1,
			  (GDestroyNotify) g_object_unref);
  gtk_widget_show (vbox1);
  gtk_container_add (GTK_CONTAINER (main_window), vbox1);
#endif

#ifdef USE_GNOME
  gnome_app_create_toolbar (GNOME_APP (main_window), toolbar);
  gnome_app_create_menus (GNOME_APP (main_window), menubar1_uiinfo);
  statusbar1 = gnome_appbar_new (FALSE, TRUE, GNOME_PREFERENCES_NEVER);
  g_object_set_data (G_OBJECT (main_window), "statusbar1", statusbar1);
  gnome_app_set_statusbar (GNOME_APP (main_window), statusbar1);
  gnome_app_install_menu_hints (GNOME_APP (main_window), menubar1_uiinfo);
#else

  gtktetcolor_factory = gtk_icon_factory_new ();

  tmp_pixbuf = gdk_pixbuf_new_from_xpm_data ((const char **) &scores_xpm);
  tmp_iconset = gtk_icon_set_new_from_pixbuf (tmp_pixbuf);
  g_object_unref (tmp_pixbuf);
  gtk_icon_factory_add (gtktetcolor_factory, "gnome-stock-scores", tmp_iconset);
  gtk_icon_set_unref (tmp_iconset);

  tmp_pixbuf = gdk_pixbuf_new_from_xpm_data ((const char **) &pause_xpm);
  tmp_iconset = gtk_icon_set_new_from_pixbuf (tmp_pixbuf);
  g_object_unref (tmp_pixbuf);
  gtk_icon_factory_add (gtktetcolor_factory, "gnome-stock-timer-stop",
			tmp_iconset);
  gtk_icon_set_unref (tmp_iconset);

  gtk_icon_factory_add_default (gtktetcolor_factory);

# ifdef ITEMFACTORY
  item_factory =
    gtk_item_factory_new (GTK_TYPE_MENU_BAR, "<main>", accel_group);
  gtk_item_factory_create_items (item_factory, G_N_ELEMENTS (menu_items),
				 menu_items, NULL);
  menubar1 = gtk_item_factory_get_widget (item_factory, "<main>");

# else
  menubar1 = gtk_menu_bar_new ();
# endif
  gtk_widget_show (menubar1);
  gtk_box_pack_start (GTK_BOX (vbox1), menubar1, FALSE, TRUE, 0);

# ifndef ITEMFACTORY
  Game = gtk_image_menu_item_new_with_mnemonic (_("_Game"));
  gtk_widget_show (Game);
  gtk_container_add (GTK_CONTAINER (menubar1), Game);

  Game_menu = gtk_menu_new ();
  gtk_menu_item_set_submenu (GTK_MENU_ITEM (Game), Game_menu);

/*
  new = gtk_image_menu_item_new_from_stock (GTK_STOCK_NEW, accel_group);
 */
  new = gtk_image_menu_item_new_with_mnemonic (_("_New game"));
  gtk_widget_show (new);
  gtk_container_add (GTK_CONTAINER (Game_menu), new);
  gtk_widget_add_accelerator (new, "activate", accel_group,
			      GDK_N, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
  tmp_toolbar_icon =
    gtk_image_new_from_stock (GTK_STOCK_NEW, GTK_ICON_SIZE_MENU);
  gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (new), tmp_toolbar_icon);

  pause = gtk_image_menu_item_new_with_mnemonic (_("_Pause"));
  gtk_widget_show (pause);
  gtk_container_add (GTK_CONTAINER (Game_menu), pause);
  gtk_widget_add_accelerator (pause, "activate", accel_group,
			      GDK_P, 0, GTK_ACCEL_VISIBLE);
  tmp_toolbar_icon =
    gtk_image_new_from_stock ("gnome-stock-timer-stop", GTK_ICON_SIZE_MENU);
  gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (pause), tmp_toolbar_icon);

  scores = gtk_image_menu_item_new_with_label (_("Scores"));
  gtk_widget_show (scores);
  gtk_container_add (GTK_CONTAINER (Game_menu), scores);
  tmp_toolbar_icon =
    gtk_image_new_from_stock ("gnome-stock-scores", GTK_ICON_SIZE_MENU);
  gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (scores),
				 tmp_toolbar_icon);

  preferences =
    gtk_image_menu_item_new_from_stock (GTK_STOCK_PROPERTIES, accel_group);
  gtk_widget_show (preferences);
  gtk_container_add (GTK_CONTAINER (Game_menu), preferences);

  quit = gtk_image_menu_item_new_from_stock (GTK_STOCK_QUIT, accel_group);
  gtk_widget_show (quit);
  gtk_container_add (GTK_CONTAINER (Game_menu), quit);

  Help = gtk_image_menu_item_new_from_stock (GTK_STOCK_HELP, accel_group);
  gtk_widget_show (Help);
  gtk_container_add (GTK_CONTAINER (menubar1), Help);

  Help_menu = gtk_menu_new ();
  gtk_menu_item_set_submenu (GTK_MENU_ITEM (Help), Help_menu);

  on_keys = gtk_image_menu_item_new_with_label (_("on keys"));
  g_object_ref (G_OBJECT (on_keys));
  g_object_set_data_full (G_OBJECT (main_window), "on_keys", on_keys,
			  (GDestroyNotify) g_object_unref);
  gtk_widget_show (on_keys);
  gtk_container_add (GTK_CONTAINER (Help_menu), on_keys);

  about = gtk_image_menu_item_new_with_mnemonic (_("_About"));
  gtk_widget_show (about);
  gtk_container_add (GTK_CONTAINER (Help_menu), about);
  tmp_toolbar_icon =
    gtk_image_new_from_stock (GTK_STOCK_ABOUT, GTK_ICON_SIZE_MENU);
  gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (about), tmp_toolbar_icon);
# endif

  toolbar1 = gtk_toolbar_new ();
  g_object_set_data (G_OBJECT (main_window), "toolbar", toolbar1);
  if (text_toolbar)
    gtk_toolbar_set_style (GTK_TOOLBAR (toolbar1), GTK_TOOLBAR_BOTH);
  else
    gtk_toolbar_set_style (GTK_TOOLBAR (toolbar1), GTK_TOOLBAR_ICONS);
  gtk_widget_show (toolbar1);
  gtk_box_pack_start (GTK_BOX (vbox1), toolbar1, FALSE, TRUE, 0);
  tooltips = gtk_tooltips_new ();

/*   tmp_toolbar_icon =
    gtk_image_new_from_stock (GTK_STOCK_NEW, GTK_ICON_SIZE_LARGE_TOOLBAR); */
  new_button = gtk_tool_button_new_from_stock (GTK_STOCK_NEW);
  /* gtk_toolbar_append_item (GTK_TOOLBAR (toolbar1), _("New"), _("New game"),
     NULL, tmp_toolbar_icon,
     G_CALLBACK (on_new_activate), NULL); */
  g_signal_connect (GTK_WIDGET (new_button), "clicked",
		    G_CALLBACK (on_new_activate), NULL);
  gtk_tool_item_set_tooltip (new_button, tooltips, _("New game"), NULL);
  gtk_tool_button_set_label (GTK_TOOL_BUTTON (new_button), _("New game"));
  gtk_toolbar_insert (GTK_TOOLBAR (toolbar1), new_button, -1);
/*  gtk_widget_show (new_button); */

  scores_button = gtk_tool_button_new_from_stock ("gnome-stock-scores");
  gtk_tool_item_set_tooltip (scores_button, tooltips, _("Scores"),
			     _("top scores"));
  gtk_tool_button_set_label (GTK_TOOL_BUTTON (scores_button), _("Scores"));
  g_signal_connect (GTK_WIDGET (scores_button), "clicked",
		    G_CALLBACK (on_scores_activate), NULL);
  gtk_toolbar_insert (GTK_TOOLBAR (toolbar1), scores_button, -1);

  pause_button = gtk_tool_button_new_from_stock ("gnome-stock-timer-stop");
  gtk_tool_item_set_tooltip (pause_button, tooltips, _("Pause game"),
			     _("Temporarily stop the game"));
  gtk_tool_button_set_label (GTK_TOOL_BUTTON (pause_button), _("Pause"));
  g_signal_connect (GTK_WIDGET (pause_button), "clicked",
		    G_CALLBACK (on_pause_activate), NULL);
  gtk_toolbar_insert (GTK_TOOLBAR (toolbar1), pause_button, -1);

  preferences_button = gtk_tool_button_new_from_stock (GTK_STOCK_PREFERENCES);
  gtk_tool_item_set_tooltip (preferences_button, tooltips, _("Preferences"),
			     NULL);
  g_signal_connect (GTK_WIDGET (preferences_button), "clicked",
		    G_CALLBACK (on_preferences_activate), NULL);
  gtk_toolbar_insert (GTK_TOOLBAR (toolbar1), preferences_button, -1);

  quit_button = gtk_tool_button_new_from_stock (GTK_STOCK_QUIT);
  gtk_tool_item_set_tooltip (quit_button, tooltips, _("Quit game"), NULL);
  g_signal_connect (GTK_WIDGET (quit_button), "clicked",
		    G_CALLBACK (on_quit_activate), NULL);
  gtk_toolbar_insert (GTK_TOOLBAR (toolbar1), quit_button, -1);
#endif

  working_hbox = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (working_hbox);
#ifndef USE_GNOME
  gtk_box_pack_start (GTK_BOX (vbox1), working_hbox, TRUE, TRUE, 0);
#endif

  left_margin_vbox = gtk_vbox_new (FALSE, 5);
/* 
  g_object_ref (G_OBJECT (left_margin_vbox));
  g_object_set_data_full (G_OBJECT (main_window), "left_margin_vbox",
			    left_margin_vbox,
			    (GDestroyNotify) g_object_unref);
 */
  gtk_widget_show (left_margin_vbox);
  gtk_box_pack_start (GTK_BOX (working_hbox), left_margin_vbox, TRUE, TRUE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (left_margin_vbox), 10);

  bonus_label = gtk_label_new ("");
  g_object_ref (G_OBJECT (bonus_label));
  g_object_set_data_full (G_OBJECT (main_window), label_name[0],
			  bonus_label, (GDestroyNotify) g_object_unref);
  gtk_widget_show (bonus_label);
  gtk_box_pack_start (GTK_BOX (left_margin_vbox), bonus_label, FALSE, FALSE, 0);

  bonus_label_number = gtk_label_new ("");
  g_object_ref (G_OBJECT (bonus_label_number));
  g_object_set_data_full (G_OBJECT (main_window), label_name[1],
			  bonus_label_number, (GDestroyNotify) g_object_unref);
  gtk_widget_show (bonus_label_number);
  gtk_box_pack_start (GTK_BOX (left_margin_vbox), bonus_label_number, FALSE,
		      FALSE, 0);

  level_label = gtk_label_new (_("Level"));
  g_object_ref (G_OBJECT (level_label));
  g_object_set_data_full (G_OBJECT (main_window), label_name[2],
			  level_label, (GDestroyNotify) g_object_unref);
  gtk_widget_show (level_label);
  gtk_box_pack_start (GTK_BOX (left_margin_vbox), level_label, FALSE, FALSE, 0);

  label_level_number = gtk_label_new ("");
  g_object_ref (G_OBJECT (label_level_number));
  g_object_set_data_full (G_OBJECT (main_window), label_name[3],
			  label_level_number, (GDestroyNotify) g_object_unref);
  gtk_widget_show (label_level_number);
  gtk_box_pack_start (GTK_BOX (left_margin_vbox), label_level_number, FALSE,
		      FALSE, 0);

  score_label = gtk_label_new (_("Score"));
  gtk_widget_set_name (score_label, label_name[4]);
  g_object_ref (G_OBJECT (score_label));
  g_object_set_data_full (G_OBJECT (main_window), label_name[4],
			  score_label, (GDestroyNotify) g_object_unref);
  gtk_widget_show (score_label);
  gtk_box_pack_start (GTK_BOX (left_margin_vbox), score_label, FALSE, FALSE, 0);

  score_label_number = gtk_label_new ("0");
  g_object_ref (G_OBJECT (score_label_number));
  g_object_set_data_full (G_OBJECT (main_window), label_name[5],
			  score_label_number, (GDestroyNotify) g_object_unref);
  gtk_widget_show (score_label_number);
  gtk_box_pack_start (GTK_BOX (left_margin_vbox), score_label_number, FALSE,
		      FALSE, 0);

  empty_label = gtk_label_new ("");
  g_object_ref (G_OBJECT (empty_label));
  g_object_set_data_full (G_OBJECT (main_window), label_name[MAX_LABEL - 1],
			  empty_label, (GDestroyNotify) g_object_unref);
  gtk_widget_show (empty_label);
  gtk_box_pack_end (GTK_BOX (left_margin_vbox), empty_label, FALSE, FALSE, 0);

  drawingarea = gtk_drawing_area_new ();
/* 
  g_object_ref (G_OBJECT (drawingarea));
  g_object_set_data_full (G_OBJECT (main_window), "drawingarea",
			    drawingarea, (GDestroyNotify) g_object_unref);
 */
  gtk_widget_show (drawingarea);
  gtk_box_pack_start (GTK_BOX (working_hbox), drawingarea, TRUE, TRUE, 0);
  gtk_widget_set_size_request (drawingarea, X_SIZE * cell_width + 2 * border,
			       Y_SIZE * cell_width + 2 * border);

  right_margin_vbox = gtk_vbox_new (FALSE, 5);
/* 
  g_object_ref (G_OBJECT (right_margin_vbox));
  g_object_set_data_full (G_OBJECT (main_window), "right_margin_vbox",
			    right_margin_vbox,
			    (GDestroyNotify) g_object_unref);
 */
  gtk_widget_show (right_margin_vbox);
  gtk_box_pack_start (GTK_BOX (working_hbox), right_margin_vbox, TRUE, TRUE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (right_margin_vbox), 10);

  max_score_label = gtk_label_new (_("max score"));
  g_object_ref (G_OBJECT (max_score_label));
  g_object_set_data_full (G_OBJECT (main_window), label_name[6],
			  max_score_label, (GDestroyNotify) g_object_unref);
  gtk_widget_show (max_score_label);
  gtk_box_pack_start (GTK_BOX (right_margin_vbox), max_score_label, FALSE,
		      FALSE, 0);

  max_score_number_label = gtk_label_new ("");
  g_object_ref (G_OBJECT (max_score_number_label));
  g_object_set_data_full (G_OBJECT (main_window), label_name[7],
			  max_score_number_label,
			  (GDestroyNotify) g_object_unref);
  gtk_widget_show (max_score_number_label);
  gtk_box_pack_start (GTK_BOX (right_margin_vbox), max_score_number_label,
		      FALSE, FALSE, 0);

  timer_label = gtk_label_new ("");
  g_object_ref (G_OBJECT (timer_label));
  g_object_set_data_full (G_OBJECT (main_window), label_name[8],
			  timer_label, (GDestroyNotify) g_object_unref);
  gtk_widget_show (timer_label);
  gtk_box_pack_end (GTK_BOX (right_margin_vbox), timer_label, FALSE, FALSE, 0);

  change_font_of_labels ();

#ifndef USE_GNOME
  statusbar1 = gtk_statusbar_new ();
  g_object_ref (G_OBJECT (statusbar1));
  g_object_set_data_full (G_OBJECT (main_window), "statusbar1",
			  statusbar1, (GDestroyNotify) g_object_unref);
  gtk_widget_show (statusbar1);
  gtk_box_pack_start (GTK_BOX (vbox1), statusbar1, FALSE, TRUE, 0);
#endif

  g_signal_connect (G_OBJECT (main_window), "delete-event",
		    G_CALLBACK (on_main_window_delete_event), NULL);
  g_signal_connect (G_OBJECT (main_window), "key-press-event",
		    G_CALLBACK (on_main_window_key_press_event), NULL);
#if !defined (USE_GNOME) && !defined (ITEMFACTORY)
  g_signal_connect (GTK_WIDGET (new), "activate",
		    G_CALLBACK (on_new_activate), NULL);
  g_signal_connect (GTK_WIDGET (pause), "activate",
		    G_CALLBACK (on_pause_activate), NULL);
  g_signal_connect (GTK_WIDGET (scores), "activate",
		    G_CALLBACK (on_scores_activate), NULL);
  g_signal_connect (GTK_WIDGET (preferences), "activate",
		    G_CALLBACK (on_preferences_activate), NULL);
  g_signal_connect (GTK_WIDGET (quit), "activate",
		    G_CALLBACK (on_quit_activate), NULL);
  g_signal_connect (GTK_WIDGET (on_keys), "activate",
		    G_CALLBACK (help_on_keys_activate), NULL);
  g_signal_connect (GTK_WIDGET (about), "activate",
		    G_CALLBACK (create_about_dialog), NULL);
#endif
  g_signal_connect (G_OBJECT (drawingarea), "expose-event",
		    G_CALLBACK (on_drawingarea_expose_event), NULL);
#ifdef USE_GNOME
  gnome_app_set_contents (GNOME_APP (main_window), working_hbox);
#else
  gtk_window_add_accel_group (GTK_WINDOW (main_window), accel_group);
#endif
  gtk_widget_grab_focus (drawingarea);
  gtk_widget_show_all (main_window);
  return main_window;
}

void
create_about_dialog (void)
{
  const gchar *authors[] = {
    _("Andrey V. Panov"),
    "<panov@canopus.iacp.dvo.ru>",
    NULL
  };
/*
  const gchar *documenters[] = {
    NULL
  };
 */

  const gchar *translator_credits = _("translator_credits");


  GtkWidget *dialog;

/*
  dialog = gnome_about_new ("gtktetcolor", VERSION,
			    _("(C) 1999-2004"),
			    _("This program is covered by the GNU GPL"),
			    authors,
			    documenters,
			    strcmp (translator_credits,
				    "translator_credits") !=
			    0 ? translator_credits : NULL, icon_xpm);
  gtk_show_about_dialog (GTK_WINDOW (main_window), "authors", authors,
                         "comments", _("This program is covered by the GNU GPL"),
			 "copyright", "(C) 1999-2004", 
//			 "translator-credits", translator_credits,
//				                       "translator_credits") !=
//			 0 ? translator_credits : NULL, 
			 "version", VERSION,
			 "website", "http://canopus.iacp.dvo.ru/~panov/gtktetcolor/");
 */
  dialog = gtk_about_dialog_new ();
  gtk_about_dialog_set_name (GTK_ABOUT_DIALOG (dialog), "gtktetcolor");
  gtk_about_dialog_set_version (GTK_ABOUT_DIALOG (dialog), VERSION);
  gtk_about_dialog_set_copyright (GTK_ABOUT_DIALOG (dialog), "(C) 1999-2004");
  gtk_about_dialog_set_comments (GTK_ABOUT_DIALOG (dialog), _("This program is covered by the GNU GPL"));
  gtk_about_dialog_set_website (GTK_ABOUT_DIALOG (dialog), "http://canopus.iacp.dvo.ru/~panov/gtktetcolor/");
  gtk_about_dialog_set_authors (GTK_ABOUT_DIALOG (dialog), authors);
  if (strcmp (translator_credits, "translator_credits"))
    gtk_about_dialog_set_translator_credits (GTK_ABOUT_DIALOG (dialog), translator_credits);
  gtk_about_dialog_set_logo (GTK_ABOUT_DIALOG (dialog), icon_xpm);
  gtk_about_dialog_set_license (GTK_ABOUT_DIALOG (dialog), NULL); // Should be GPL
  gtk_window_set_transient_for (GTK_WINDOW (dialog), GTK_WINDOW (main_window));
  gtk_widget_show_all (dialog);
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

  g_signal_connect_swapped (GTK_OBJECT (dialog),
			    "response",
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
    gtk_table_attach_defaults (GTK_TABLE (scores_table), name_label[i], 0, 1,
			       i, i + 1);
    gtk_misc_set_alignment (GTK_MISC (name_label[i]), 0., GTK_MISC
			    (name_label[i])->yalign);
    gtk_table_attach_defaults (GTK_TABLE (scores_table), score_label[i], 1,
			       2, i, i + 1);
    gtk_misc_set_alignment (GTK_MISC (score_label[i]), 1., GTK_MISC
			    (score_label[i])->yalign);
  }

  gtk_widget_show_all (dialog);
}

void
create_name_dialog (void)
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

  g_signal_connect_swapped (GTK_OBJECT (dialog),
			    "response",
			    G_CALLBACK (on_name_response), GTK_OBJECT (dialog));

  name_frame = gtk_frame_new (_("Enter your name"));
  gtk_container_set_border_width (GTK_CONTAINER (name_frame), 5);
  gtk_container_add (GTK_CONTAINER (GTK_DIALOG (dialog)->vbox), name_frame);

  name_entry = gtk_entry_new ();
  gtk_entry_set_max_length (GTK_ENTRY (name_entry), 255);
  if (strlen (new_name) != 0) {
    gtk_entry_set_text (GTK_ENTRY (name_entry), new_name);
    gtk_editable_select_region (GTK_EDITABLE (name_entry), 0,
				strlen (new_name));
  }
  g_object_ref (G_OBJECT (name_entry));
  g_object_set_data_full (G_OBJECT (dialog), "name_entry",
			  name_entry, (GDestroyNotify) g_object_unref);
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

  g_signal_connect_swapped (GTK_OBJECT (dialog),
			    "response",
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
					GTK_DIALOG_MODAL |
					GTK_DIALOG_DESTROY_WITH_PARENT,
					GTK_STOCK_OK, GTK_RESPONSE_NONE, NULL);
  gtk_window_set_resizable (GTK_WINDOW (dialog), FALSE);

  g_signal_connect_swapped (GTK_OBJECT (dialog),
			    "response",
			    G_CALLBACK (gtk_widget_destroy),
			    GTK_OBJECT (dialog));

  if (use_graykeys)
    label = gtk_label_new (_("Left arrow  - shift block left\n"
			     "Down arrow  - rotate block clockwise\n"
			     "Right arrow - shift block right\n"
			     "Up arrow    - rotate block counter-clockwise\n"
			     "Space - drop block"));
  else
    label = gtk_label_new (_("Num 4 - shift block left\n"
			     "Num 5 - rotate block clockwise\n"
			     "Num 6 - shift block right\n"
			     "Num 8 - rotate block counter-clockwise\n"
			     "Space - drop block"));

  gtk_misc_set_padding (GTK_MISC (label), 10, 10);

  gtk_container_add (GTK_CONTAINER (GTK_DIALOG (dialog)->vbox), label);
  gtk_widget_show_all (dialog);
}

void
create_preferences_dialog (void)
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
  GtkWidget *choose_keys_label;
  GtkWidget *vbox3;
  GSList *keys_button_group = NULL;
  GtkWidget *numkeys_radiobutton;
  GtkWidget *graykeys_radiobutton;
  GtkWidget *prefs_notebook;
  GtkWidget *options_label;
  GtkWidget *fontselection;
  GtkWidget *fonts_label;

#ifdef USE_GNOME
  GtkWidget *sound_hbox;
  GtkWidget *sound_checkbutton;
#else
  GtkWidget *text_toolbar_hbox;
  GtkWidget *text_toolbar_checkbutton;
#endif

  preferences_dialog =
    gtk_dialog_new_with_buttons (_("GtkTetcolor preferences"),
				 GTK_WINDOW (main_window),
				 GTK_DIALOG_DESTROY_WITH_PARENT, GTK_STOCK_OK,
				 GTK_RESPONSE_OK, GTK_STOCK_APPLY,
				 GTK_RESPONSE_APPLY, GTK_STOCK_CLOSE,
				 GTK_RESPONSE_CLOSE, NULL);
  g_signal_connect_swapped (GTK_OBJECT (preferences_dialog), "response",
			    G_CALLBACK (preferences_dialog_response),
			    GTK_OBJECT (preferences_dialog));

  prefs_notebook = gtk_notebook_new ();
  gtk_box_pack_start (GTK_BOX (GTK_DIALOG (preferences_dialog)->vbox),
		      prefs_notebook, FALSE, TRUE, 10);

  preferences_vbox = gtk_vbox_new (FALSE, 5);
  options_label = gtk_label_new (_("Game options"));

  gtk_container_add (GTK_CONTAINER (prefs_notebook), preferences_vbox);
  gtk_notebook_set_tab_label (GTK_NOTEBOOK (prefs_notebook),
			      gtk_notebook_get_nth_page (GTK_NOTEBOOK
							 (prefs_notebook), 0),
			      options_label);

  fontselection = gtk_font_selection_new ();
  g_object_ref (G_OBJECT (fontselection));
  g_object_set_data_full (G_OBJECT (preferences_dialog), "fontselection",
			  fontselection, (GDestroyNotify) g_object_unref);

  fonts_label = gtk_label_new (_("Font for labels"));
  gtk_container_add (GTK_CONTAINER (prefs_notebook), fontselection);
  gtk_notebook_set_tab_label (GTK_NOTEBOOK (prefs_notebook),
			      gtk_notebook_get_nth_page (GTK_NOTEBOOK
							 (prefs_notebook), 1),
			      fonts_label);

  preferences_hbox1 = gtk_hbox_new (FALSE, 0);
  gtk_box_pack_start (GTK_BOX (preferences_vbox), preferences_hbox1, FALSE,
		      TRUE, 10);

  cell_size_label = gtk_label_new (_("Cell size: "));
  gtk_box_pack_start (GTK_BOX (preferences_hbox1), cell_size_label, FALSE,
		      FALSE, 10);

  cell_size_spinbutton_adj =
    gtk_adjustment_new ((gfloat) cell_width, 10, MAX_CELL_SIZE, 1, 10, 10);
  cell_size_spinbutton =
    gtk_spin_button_new (GTK_ADJUSTMENT (cell_size_spinbutton_adj), 1, 0);
  g_object_ref (G_OBJECT (cell_size_spinbutton));
  g_object_set_data_full (G_OBJECT (preferences_dialog),
			  "cell_size_spinbutton", cell_size_spinbutton,
			  (GDestroyNotify) g_object_unref);

  gtk_box_pack_start (GTK_BOX (preferences_hbox1), cell_size_spinbutton,
		      FALSE, TRUE, 10);

  preferences_hbox2 = gtk_hbox_new (FALSE, 0);
  gtk_box_pack_start (GTK_BOX (preferences_vbox), preferences_hbox2, FALSE,
		      TRUE, 10);

  initial_level_label = gtk_label_new (_("Initial level: "));
  gtk_box_pack_start (GTK_BOX (preferences_hbox2), initial_level_label, FALSE,
		      FALSE, 10);

  initial_level_spinbutton_adj =
    gtk_adjustment_new ((gfloat) initial_level, 1, 9, 1, 10, 10);
  initial_level_spinbutton =
    gtk_spin_button_new (GTK_ADJUSTMENT (initial_level_spinbutton_adj), 1, 0);
  g_object_ref (G_OBJECT (initial_level_spinbutton));
  g_object_set_data_full (G_OBJECT (preferences_dialog),
			  "initial_level_spinbutton",
			  initial_level_spinbutton,
			  (GDestroyNotify) g_object_unref);
  gtk_box_pack_start (GTK_BOX (preferences_hbox2), initial_level_spinbutton,
		      FALSE, TRUE, 10);

  preferences_destroy_delay_hbox = gtk_hbox_new (FALSE, 0);
  gtk_box_pack_start (GTK_BOX (preferences_vbox),
		      preferences_destroy_delay_hbox, FALSE, TRUE, 10);

  destroy_delay_label = gtk_label_new (_("Delay before cell destroying, ms: "));
  gtk_box_pack_start (GTK_BOX (preferences_destroy_delay_hbox),
		      destroy_delay_label, FALSE, FALSE, 10);

  destroy_delay_spinbutton_adj =
    gtk_adjustment_new ((gfloat) destroy_delay, 5, MAX_DESTROY_DELAY, 5, 10,
			10);
  destroy_delay_spinbutton =
    gtk_spin_button_new (GTK_ADJUSTMENT (destroy_delay_spinbutton_adj), 1, 0);
  g_object_ref (G_OBJECT (destroy_delay_spinbutton));
  g_object_set_data_full (G_OBJECT (preferences_dialog),
			  "destroy_delay_spinbutton",
			  destroy_delay_spinbutton,
			  (GDestroyNotify) g_object_unref);
  gtk_box_pack_start (GTK_BOX (preferences_destroy_delay_hbox),
		      destroy_delay_spinbutton, FALSE, TRUE, 10);

  preferences_hbox3 = gtk_hbox_new (FALSE, 0);
  gtk_box_pack_start (GTK_BOX (preferences_vbox), preferences_hbox3, FALSE,
		      TRUE, 10);

  choose_keys_label = gtk_label_new (_("Choose keys"));
  gtk_box_pack_start (GTK_BOX (preferences_hbox3), choose_keys_label, FALSE,
		      FALSE, 10);
  gtk_misc_set_alignment (GTK_MISC (choose_keys_label), 0.5, 0.1);

  vbox3 = gtk_vbox_new (FALSE, 0);
  gtk_box_pack_start (GTK_BOX (preferences_hbox3), vbox3, FALSE, FALSE, 10);

  numkeys_radiobutton =
    gtk_radio_button_new_with_label (keys_button_group, _("numeric keyboard"));
  keys_button_group =
    gtk_radio_button_get_group (GTK_RADIO_BUTTON (numkeys_radiobutton));
  g_object_ref (G_OBJECT (numkeys_radiobutton));
  g_object_set_data_full (G_OBJECT (preferences_dialog),
			  "numkeys_radiobutton", numkeys_radiobutton,
			  (GDestroyNotify) g_object_unref);
  gtk_box_pack_start (GTK_BOX (vbox3), numkeys_radiobutton, FALSE, FALSE, 0);

  graykeys_radiobutton =
    gtk_radio_button_new_with_label (keys_button_group, _("arrow keys"));
  keys_button_group =
    gtk_radio_button_get_group (GTK_RADIO_BUTTON (graykeys_radiobutton));
  g_object_ref (G_OBJECT (graykeys_radiobutton));
  g_object_set_data_full (G_OBJECT (preferences_dialog),
			  "graykeys_radiobutton", graykeys_radiobutton,
			  (GDestroyNotify) g_object_unref);
  gtk_box_pack_end (GTK_BOX (vbox3), graykeys_radiobutton, FALSE, FALSE, 0);

  if (use_graykeys)
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (graykeys_radiobutton),
				  TRUE);

#ifdef USE_GNOME
  sound_hbox = gtk_hbox_new (FALSE, 0);
  gtk_box_pack_start (GTK_BOX (preferences_vbox), sound_hbox, FALSE, FALSE, 10);

  sound_checkbutton = gtk_check_button_new_with_label (_("Use sound"));
  g_object_ref (G_OBJECT (sound_checkbutton));
  g_object_set_data_full (G_OBJECT (preferences_dialog),
			  "sound_checkbutton", sound_checkbutton,
			  (GDestroyNotify) g_object_unref);
  gtk_box_pack_start (GTK_BOX (sound_hbox), sound_checkbutton, FALSE, FALSE,
		      10);

  if (sound_on)
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (sound_checkbutton), TRUE);
  else
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (sound_checkbutton), FALSE);

  g_signal_connect (G_OBJECT (sound_checkbutton), "toggled",
		    G_CALLBACK (on_sound_checkbutton_toggled), NULL);
#else
  text_toolbar_hbox = gtk_hbox_new (FALSE, 0);
  gtk_box_pack_start (GTK_BOX (preferences_vbox), text_toolbar_hbox, FALSE,
		      FALSE, 10);

  text_toolbar_checkbutton =
    gtk_check_button_new_with_label (_("Show text in toolbar"));
  g_object_ref (G_OBJECT (text_toolbar_checkbutton));
  g_object_set_data_full (G_OBJECT (preferences_dialog),
			  "text_toolbar_checkbutton", text_toolbar_checkbutton,
			  (GDestroyNotify) g_object_unref);
  gtk_box_pack_start (GTK_BOX (text_toolbar_hbox), text_toolbar_checkbutton,
		      FALSE, FALSE, 10);

  if (text_toolbar)
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (text_toolbar_checkbutton),
				  TRUE);
  else
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (text_toolbar_checkbutton),
				  FALSE);

  g_signal_connect (G_OBJECT (text_toolbar_checkbutton), "toggled",
		    G_CALLBACK (on_text_toolbar_checkbutton_toggled), NULL);
#endif
  g_signal_connect (G_OBJECT (numkeys_radiobutton), "clicked",
		    G_CALLBACK (on_numkeys_radiobutton_clicked), NULL);
  g_signal_connect (G_OBJECT (graykeys_radiobutton), "clicked",
		    G_CALLBACK (on_graykeys_radiobutton_clicked), NULL);

  gtk_widget_show_all (preferences_dialog);
  
  if (font_name)
    gtk_font_selection_set_font_name (GTK_FONT_SELECTION (fontselection),
				      font_name);
}
