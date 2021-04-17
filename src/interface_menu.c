/*
 * This is free and unencumbered software released into the public domain.
 *
 * For more information, please refer to <https://unlicense.org>
 */

static GtkUIManager   * ui_manager;
static GtkActionGroup * menu_actions;
static GtkWidget      * menubar;

static GtkActionEntry menu_items[] =
{
   //  name        stock_id          "label"         accel      tooltip  callback
   { "GameMenu",   NULL,            N_("_Game"),    NULL,         NULL, NULL },
   { "GameNew",    GTK_STOCK_NEW,   N_("_New"),     "<control>N", NULL, G_CALLBACK(on_new_activate) },
   { "GamePause",  NULL,            N_("_Pause"),   "P",          NULL, G_CALLBACK(on_pause_activate) },
   { "GameScores", NULL,            N_("_Scores"),  NULL,         NULL, G_CALLBACK(on_scores_activate) },
   { "GamePrefs",  GTK_STOCK_PROPERTIES, N_("_Preferences"),NULL, NULL, G_CALLBACK(on_preferences_activate) },
   { "GameQuit",   GTK_STOCK_QUIT,  N_("_Quit"),   "<control>Q",  NULL, G_CALLBACK(on_quit_activate) },

   { "HelpMenu",   NULL,            N_("_Help"),    NULL,         NULL, NULL },
   { "HelpOnkeys", NULL,            N_("on keys"),  NULL,         NULL, G_CALLBACK(help_on_keys_activate) },
   { "HelpAbout",  GTK_STOCK_ABOUT, N_("_About"),   NULL,         NULL, G_CALLBACK(create_about_dialog) },
};

static const char * menu_ui_str = 
"<ui>"
"  <menubar name='menubar'>"
"    <menu action='GameMenu'>"
"      <menuitem action='GameNew'/>"
"      <menuitem action='GamePause'/>"
"      <menuitem action='GameScores'/>"
"      <separator/>"
"      <menuitem action='GamePrefs'/>"
"      <separator/>"
"      <menuitem action='GameQuit'/>"
"    </menu>"
"    <menu action='HelpMenu'>"
"      <menuitem action='HelpOnkeys'/>"
"      <menuitem action='HelpAbout'/>"
"    </menu>"
"  </menubar>"
"</ui>";


static char * menu_translate (const char * path, gpointer data)
{
   return gettext (path);
}


static void create_menubar (GtkBox * box, GtkWindow * window)
{
   GtkAccelGroup * accel_group;

   menu_actions = gtk_action_group_new ("MenuActions");
   gtk_action_group_set_translate_func (menu_actions,
                                        menu_translate, NULL, NULL);
   gtk_action_group_add_actions (menu_actions,
                                 menu_items,
                                 G_N_ELEMENTS (menu_items),
                                 window);

   ui_manager  = gtk_ui_manager_new ();
   gtk_ui_manager_insert_action_group (ui_manager, menu_actions, 0);

   accel_group = gtk_ui_manager_get_accel_group (ui_manager);
   gtk_window_add_accel_group (window, accel_group);

   gtk_ui_manager_add_ui_from_string (ui_manager, menu_ui_str, -1, NULL);

   menubar = gtk_ui_manager_get_widget (ui_manager, "/menubar");

   gtk_box_pack_start (box, GTK_WIDGET (menubar), FALSE, FALSE, 0);
}

