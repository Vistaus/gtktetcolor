/* GtkTetcolor
 * Copyright (C) 1999, 2002 Andrey V. Panov
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

#include <stdio.h>                                                              

#ifndef USE_GNOME  
FILE *score_file;
gchar *score_path;
gchar *rcfile_path;
gint name_i;

gboolean read_score (void);

gboolean choose_score_file (void);

gboolean insert_new_score (void);

gboolean write_score (void);

gint check_line (gint i);
#endif
