/* gks-gpg.h
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

#ifndef GKS_GPG_H
#define GKS_GPG_H

#include <glib-object.h>

G_BEGIN_DECLS

#define GKS_TYPE_GPG            (gks_gpg_get_type())

G_DECLARE_FINAL_TYPE (GksGpg, gks_gpg, GKS, GPG, GObject)

typedef struct _GksGpgPrivate GksGpgPrivate;

struct _GksGpg
{
  GObject parent;

  /*< private >*/
  GksGpgPrivate *priv;
};

GksGpg     *gks_gpg_new      (void);

GPtrArray *gks_gpg_list_keys (GksGpg  *self);

G_END_DECLS

#endif /* GKS_GPG_H */
