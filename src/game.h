/* GtkTetcolor
 * Copyright (C) 1999 - 2001 Andrey V. Panov
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

void init_game (GtkWidget * widget);

void game_over (GtkWidget * widget);

gboolean
timeout_callback ( gpointer data );

gboolean rotate_cell_right (gint i, gint * newx, gint * newy);

gboolean rotate_cell_left (gint i, gint * newx, gint * newy);

void change_rotated_cell (gint i, gint newx, gint newy);

gboolean change_level ( gpointer data );

void check_lines (GtkWidget * widget);

gboolean seek_removing_cells (void);

gboolean remove_cells (void);

gint delay_moving (void);

gboolean rotate_block (gboolean (*rotate_cell)(gint i, gint *newx, gint *newy));

gboolean shift_block (gint direction);

