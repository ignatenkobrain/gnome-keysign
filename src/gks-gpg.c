/* gks-gpg.c
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

#include <gpgme.h>

#include "gks-gpg.h"

struct _GksGpgPrivate
{
  gpgme_ctx_t ctx;
};

G_DEFINE_TYPE_WITH_PRIVATE (GksGpg, gks_gpg, G_TYPE_OBJECT)

enum {
  PROP_0,
  LAST_PROP
};

static GParamSpec *gParamSpecs [LAST_PROP];

GksGpg *
gks_gpg_new (void)
{
  return g_object_new (GKS_TYPE_GPG, NULL);
}

/**
 * gks_gpg_list_keys:
 * @self: #GksGpg
 *
 * Returns: %GPtrArray with list of found keys
 */
GPtrArray *
gks_gpg_list_keys (GksGpg   *self)
{
  GksGpgPrivate *priv = self->priv;
  gpgme_key_t key;
  gpgme_error_t err;
  GPtrArray *keys = g_ptr_array_new_with_free_func ((GDestroyNotify) gpgme_key_unref);

  err = gpgme_op_keylist_start (priv->ctx, NULL, 0);
  while (!err) {
    err = gpgme_op_keylist_next (priv->ctx, &key);
    if (err)
      break;
    g_debug ("Found key: %s", key->subkeys->keyid);
    g_ptr_array_add (keys, (gpointer) key);
  }
  if (gpg_err_code (err) != GPG_ERR_EOF) {
    g_warning ("Can not list keys: %s\n", gpgme_strerror (err));
  }
  return keys;
}

static void
gks_gpg_finalize (GObject *object)
{
  GksGpgPrivate *priv = GKS_GPG (object)->priv;

  gpgme_release (priv->ctx);

  G_OBJECT_CLASS (gks_gpg_parent_class)->finalize (object);
}

static void
gks_gpg_get_property (GObject    *object,
                      guint       prop_id,
                      GValue     *value,
                      GParamSpec *pspec)
{
  GksGpg *self = GKS_GPG (object);

  switch (prop_id)
    {
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
gks_gpg_set_property (GObject      *object,
                      guint         prop_id,
                      const GValue *value,
                      GParamSpec   *pspec)
{
  GksGpg *self = GKS_GPG (object);

  switch (prop_id)
    {
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
gks_gpg_class_init (GksGpgClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = gks_gpg_finalize;
  object_class->get_property = gks_gpg_get_property;
  object_class->set_property = gks_gpg_set_property;
}

static void
gks_gpg_init (GksGpg *self)
{
  self->priv = gks_gpg_get_instance_private (self);

  gpgme_check_version (NULL);
  gpgme_new (&self->priv->ctx);
}
