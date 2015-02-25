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
#define GKS_GPG(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), GKS_TYPE_GPG, GksGpg))
#define GKS_GPG_CONST(obj)      (G_TYPE_CHECK_INSTANCE_CAST ((obj), GKS_TYPE_GPG, GksGpg const))
#define GKS_GPG_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass),  GKS_TYPE_GPG, GksGpgClass))
#define GKS_IS_GPG(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GKS_TYPE_GPG))
#define GKS_IS_GPG_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),  GKS_TYPE_GPG))
#define GKS_GPG_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),  GKS_TYPE_GPG, GksGpgClass))

typedef struct _GksGpg        GksGpg;
typedef struct _GksGpgClass   GksGpgClass;
typedef struct _GksGpgPrivate GksGpgPrivate;

struct _GksGpg
{
  GObject parent;

  /*< private >*/
  GksGpgPrivate *priv;
};

struct _GksGpgClass
{
  GObjectClass parent;
};

GType       gks_gpg_get_type (void);
GksGpg     *gks_gpg_new      (void);

GPtrArray *gks_gpg_list_keys (GksGpg  *self);

G_END_DECLS

#endif /* GKS_GPG_H */
