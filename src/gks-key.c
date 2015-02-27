/* gks-key.c
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

#include "gks-key.h"

struct _GksKeyPrivate
{
  gpgme_key_t key;
  gint times_signed;
};

G_DEFINE_TYPE_WITH_PRIVATE (GksKey, gks_key, G_TYPE_OBJECT)

enum {
  PROP_0,
  PROP_TIMES_SIGNED,
  LAST_PROP
};

static GParamSpec *gParamSpecs [LAST_PROP];

gpgme_key_t
gks_key_get_key (GksKey *key)
{
  g_return_val_if_fail (GKS_IS_KEY (key), 0);

  return key->priv->key;
}

void
gks_key_set_key (GksKey      *key,
                 gpgme_key_t  k)
{
  g_return_if_fail (GKS_IS_KEY (key));

  key->priv->key = k;
}

gint
gks_key_get_times_signed (GksKey *key)
{
  g_return_val_if_fail (GKS_IS_KEY (key), 0);

  return key->priv->times_signed;
}

void
gks_key_set_times_signed (GksKey *key,
                          gint    times_signed)
{
  g_return_if_fail (GKS_IS_KEY (key));

  key->priv->times_signed = times_signed;
}

GksKey *
gks_key_new (void)
{
  return g_object_new (GKS_TYPE_KEY, NULL);
}

static void
gks_key_finalize (GObject *object)
{
  GksKeyPrivate *priv = GKS_KEY (object)->priv;

  gpgme_key_unref (priv->key);

  G_OBJECT_CLASS (gks_key_parent_class)->finalize (object);
}

static void
gks_key_get_property (GObject    *object,
                      guint       prop_id,
                      GValue     *value,
                      GParamSpec *pspec)
{
  GksKey *self = GKS_KEY (object);

  switch (prop_id)
    {
    case PROP_TIMES_SIGNED:
      g_value_set_int (value, gks_key_get_times_signed (self));
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
gks_key_set_property (GObject      *object,
                      guint         prop_id,
                      const GValue *value,
                      GParamSpec   *pspec)
{
  GksKey *self = GKS_KEY (object);

  switch (prop_id)
    {
    case PROP_TIMES_SIGNED:
      gks_key_set_times_signed (self, g_value_get_int (value));
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
gks_key_class_init (GksKeyClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = gks_key_finalize;
  object_class->get_property = gks_key_get_property;
  object_class->set_property = gks_key_set_property;

  gParamSpecs [PROP_TIMES_SIGNED] =
    g_param_spec_int ("times_signed", "TimesSigned", "Times Signed",
                      0, G_MAXINT, 0, G_PARAM_READWRITE);
  g_object_class_install_property (object_class, PROP_TIMES_SIGNED,
                                   gParamSpecs [PROP_TIMES_SIGNED]);
}

static void
gks_key_init (GksKey *self)
{
  self->priv = gks_key_get_instance_private (self);
}
