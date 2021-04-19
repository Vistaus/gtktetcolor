/*
 * This is free and unencumbered software released into the public domain.
 *
 * For more information, please refer to <https://unlicense.org>
 */

/* --> create_menu_bar <-- */
 
#if GTK_CHECK_VERSION (3, 10, 0)

/* ----------- */
/* GTK >= 3.10 */
/* ----------- */

static void game_new_cb (GSimpleAction *simple, GVariant *parameter, gpointer user_data) {
   on_new_activate ();
}
static void game_pause_cb (GSimpleAction *simple, GVariant *parameter, gpointer user_data) {
   on_pause_activate ();
}
static void game_scores_cb (GSimpleAction *simple, GVariant *parameter, gpointer user_data) {
   on_scores_activate ();
}
static void game_prefs_cb (GSimpleAction *simple, GVariant *parameter, gpointer user_data) {
   on_preferences_activate ();
}
static void game_quit_cb (GSimpleAction *simple, GVariant *parameter, gpointer user_data) {
   on_quit_activate ();
}
static void help_onkeys_cb (GSimpleAction *simple, GVariant *parameter, gpointer user_data) {
   help_on_keys_activate ();
}
static void help_about_cb (GSimpleAction *simple, GVariant *parameter, gpointer user_data) {
   create_about_dialog ();
}

static const GActionEntry menu_actions[] =
{
   // name,      activate_callback,  param_type, state, change_state_callback
   { "GameNew",    game_new_cb    },
   { "GamePause",  game_pause_cb  },
   { "GameScores", game_scores_cb },
   { "GamePrefs",  game_prefs_cb  },
   { "GameQuit",   game_quit_cb   },
   { "HelpOnKeys", help_onkeys_cb },
   { "HelpAbout",  help_about_cb  },
};

static const char * menu_ui_str =
"<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
"<interface>"
"  <menu id='menubar'>"
"    <submenu>"
"      <attribute name='label'>_Game</attribute>"
"      <section>"
"        <item>"
"          <attribute name='label'>_New</attribute>"
"          <attribute name='action'>win.GameNew</attribute>"
"        </item>"
"        <item>"
"          <attribute name='label'>_Pause</attribute>"
"          <attribute name='action'>win.GamePause</attribute>"
"        </item>"
"        <item>"
"          <attribute name='label'>_Scores</attribute>"
"          <attribute name='action'>win.GameScores</attribute>"
"        </item>"
"        <item>"
"          <attribute name='label'>_Preferences</attribute>"
"          <attribute name='action'>win.GamePrefs</attribute>"
"        </item>"
"        <item>"
"          <attribute name='label'>Quit</attribute>"
"          <attribute name='action'>win.GameQuit</attribute>"
"        </item>"
"      </section>"
"    </submenu>"
"    <submenu>"
"      <attribute name='label'>_Help</attribute>"
"      <section>"
"        <item>"
"          <attribute name='label'>on keys</attribute>"
"          <attribute name='action'>win.HelpOnKeys</attribute>"
"        </item>"
"        <item>"
"          <attribute name='label'>About</attribute>"
"          <attribute name='action'>win.HelpAbout</attribute>"
"        </item>"
"      </section>"
"    </submenu>"
"  </menu>"
"</interface>";



static void create_menubar (GtkApplicationWindow * window, GtkApplication * app)
{
   /* GtkApplicationWindow implements GActionMap */
   GActionMap * amap = G_ACTION_MAP (window);
   g_action_map_add_action_entries (amap, menu_actions,
                                    G_N_ELEMENTS (menu_actions), NULL);
   // accels
   static const char * accel_new[] =   { "<control>N", NULL };
   static const char * accel_pause[] = { "P", NULL };
   static const char * accel_quit[] =  { "<control>Q", NULL };
   gtk_application_set_accels_for_action (app, "win.GameNew",   accel_new);
   gtk_application_set_accels_for_action (app, "win.GamePause", accel_pause);
   gtk_application_set_accels_for_action (app, "win.GameQuit",  accel_quit);

   GtkBuilder * builder = gtk_builder_new ();
   GError * error = NULL;
   gtk_builder_add_from_string (builder, menu_ui_str, -1, &error);
   if (error) {
      puts (error->message);
      g_error_free (error);
   }
   gtk_builder_set_translation_domain (builder, NULL);

   GObject * menubar = gtk_builder_get_object (builder, "menubar");
   gtk_application_set_menubar (app, G_MENU_MODEL (menubar));
}

#else

/* ---------- */
/* GTK < 3.10 */
/* ---------- */

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

#endif
