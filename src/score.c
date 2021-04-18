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
#include <sys/stat.h>

#include "score.h"
#include "interface.h"

#define SCORE_FILE "gtktetcolor_score"


gboolean read_score (void)
{
   gint i, j;
   gint cksum;
   struct stat file_info;
   char * score_path = get_config_dir_file (SCORE_FILE);
   FILE * score_file;

   if ((score_file = fopen (score_path, "r")) == NULL) {
      /* g_print ("\ncannot read file %s\n", score_path); */
      g_free (score_path);
      return FALSE;
   }
   stat (score_path, &file_info);	/* We need check if score_path is empty */
   for (i = 0; (i < 10) && (!feof (score_file)); i++)
   {
      for (j = 0; j < 256; j++) {
          fscanf (score_file, "%c", &name[i][j]);
          if (name[i][j] == '\t')
             goto a;
          else if (name[i][j] == '\n')
             fscanf (score_file, "%c", &name[i][j]);
      }
  a:
      name[i][j] = '\0';
      fscanf (score_file, "%d", &saved_score[i]);
      fscanf (score_file, "%x", &cksum);
      if (check_line (i) != cksum) {
         if (file_info.st_size) {
            g_print ("\n line No. %d in file %s is corrupted\n", i + 1, score_path);
         }
         strcpy (name[i], "<none>");
         saved_score[i] = 0;
      }
   }

   if (i < 9) {
      for (j = i; j < 10; j++) {
         name[j][0] = '\0';
         saved_score[j] = 0;
      }
   }

   g_free (score_path);
   fclose (score_file);
   return TRUE;
}


gboolean write_score (void)
{
   gint i;
   char * score_path = get_config_dir_file (SCORE_FILE);
   FILE * score_file;

   score_file = fopen (score_path, "w");
   if (!score_file) {
      g_print ("\ncannot write file %s", score_path);
      g_free (score_path);
      return FALSE;
   }

   for (i = 0; i < 10; i++) {
       if (strlen (name[i]) == 0) {
          strcpy (name[i], "<none>");
       }
       fprintf (score_file, "%s\t%d\t%x\n", name[i], saved_score[i],
       check_line (i));
   }

   g_free (score_path);
   fclose (score_file);
   return TRUE;
}


gboolean insert_new_score (void)
{
   gint i = 0, j;

   if (score <= 0) {
      return FALSE;
   }
   while (saved_score[i] >= score) {
      i++;
   }
   if (i > 9) {
      return FALSE;
   }
   for (j = 8; j >= i; j--) {
      strcpy (name[j + 1], name[j]);
      saved_score[j + 1] = saved_score[j];
   }
   name_i = i;
   create_name_dialog ();
   return TRUE;
}


gint check_line (gint i)
{
   gint j, ckline = 0;
   div_t result;
   for (j = 0; j < 256; j++) {
      ckline = ckline + (gint) name[i][j];
      if (!name[i][j]) {
         break;
      }
   }
   ckline = ckline + saved_score[i] * 3;
   result = div ((int) ckline, 1111);
   return (gint) result.rem;
}

