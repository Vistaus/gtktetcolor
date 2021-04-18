/* GtkTetcolor
 * Copyright (C) 1999, 2000 Andrey V. Panov
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <glib/gi18n.h>
#include "gtkcompat.h"


#define X_SIZE 7
#define Y_SIZE 18
#define NUMBER_COLORS 7
#define NUMBER_TYPES 4
#define INI_INTERVAL 1000
#define LEVEL_INT 1000
#define MIN_SCORE 40
#define border 2
#define MAX_CELL_SIZE 80
#define MAX_DESTROY_DELAY 150

/*
 * Standard gettext macros.
 */
// #ifndef USE_GNOME
// # ifdef ENABLE_NLS 
// #  include <libintl.h>
// #  undef _
// #  define _(String) dgettext (PACKAGE, String)
// #  ifdef gettext_noop
// #    define N_(String) gettext_noop (String)
// #  else
// #    define N_(String) (String)
// #  endif
// # else
// #  define textdomain(String) (String)
// #  define gettext(String) (String)
// #  define dgettext(Domain,Message) (Message)
// #  define dcgettext(Domain,Message,Type) (Message)
// #  define bindtextdomain(Domain,Directory) (Domain)
// #  define _(String) (String)
// #  define N_(String) (String)
// # endif
// #endif

struct
{
   gint num;
   gint type;
   gint color[3];
   gint x[3];
   gint y[3];
}
block;

gboolean new_block (void);
gint cell_state[X_SIZE][Y_SIZE];
gint cell_state_wo_block[X_SIZE][Y_SIZE];
gint cell_state_prev[X_SIZE][Y_SIZE];
gint removing_cell[X_SIZE][Y_SIZE];
gint lines, score, bonus, saved_score[10], continue_game;
gint bonus_tic;
gchar name[10][256], new_name[256];
guint32 interval;
gint timeout, level, level_tic, level_timeout;

/* Main widgets*/

GtkWidget *main_window;
GtkWidget *drawingarea;


void before_exit (void);
