/* gks-key.h
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

#ifndef GKS_KEY_H
#define GKS_KEY_H

#include <glib-object.h>
#include <gpgme.h>

G_BEGIN_DECLS

#define GKS_TYPE_KEY            (gks_key_get_type())

G_DECLARE_FINAL_TYPE (GksKey, gks_key, GKS, KEY, GObject)

typedef struct _GksKeyPrivate GksKeyPrivate;

struct _GksKey
{
  GObject parent;

  /*< private >*/
  GksKeyPrivate *priv;
};

GksKey     *gks_key_new      (void);

gpgme_key_t
gks_key_get_key (GksKey *key);
void
gks_key_set_key (GksKey      *key,
                 gpgme_key_t  k);

gint
gks_key_get_times_signed (GksKey *key);
void
gks_key_set_times_signed (GksKey *key,
                          gint    times_signed);

G_END_DECLS

#endif /* GKS_KEY_H */
