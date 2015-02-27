/* gks-row.h
 *
 * Copyright (C) 2015 Igor Gnatenko <i.gnatenko.brain@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef GKS_ROW_H
#define GKS_ROW_H

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define GKS_TYPE_ROW            (gks_row_get_type())

G_DECLARE_FINAL_TYPE (GksRow, gks_row, GKS, ROW, GtkListBoxRow)

typedef struct _GksRowPrivate GksRowPrivate;

struct _GksRow
{
  GtkListBoxRow parent;

  /*< private >*/
  GksRowPrivate *priv;
};

GtkWidget *
gks_row_new (const gchar *keyid,
             gint         times_signed,
             const gchar *description,
             const gchar *expiration);

const gchar *
gks_row_get_keyid (GksRow *row);
void
gks_row_set_keyid (GksRow      *row,
                   const gchar *keyid);

gint
gks_row_get_times_signed (GksRow *row);
void
gks_row_set_times_signed (GksRow *row,
                          gint    times_signed);

const gchar *
gks_row_get_description (GksRow *row);
void
gks_row_set_description (GksRow      *row,
                         const gchar *description);

const gchar *
gks_row_get_expiration (GksRow *row);
void
gks_row_set_expiration (GksRow      *row,
                        const gchar *date);

G_END_DECLS

#endif /* GKS_ROW_H */
