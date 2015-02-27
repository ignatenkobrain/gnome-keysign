/* gks-row.c
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

#include "gks-row.h"

struct _GksRowPrivate
{
  GtkWidget *name_label;
  GtkWidget *times_signed_label;
  GtkWidget *description_label;
  GtkWidget *expiration_label;
};

G_DEFINE_TYPE_WITH_PRIVATE (GksRow, gks_row, GTK_TYPE_LIST_BOX_ROW)

enum {
  PROP_0,
  PROP_KEYID,
  PROP_TIMES_SIGNED,
  PROP_DESCRIPTION,
  PROP_EXPIRATION,
  LAST_PROP
};

static GParamSpec *gParamSpecs [LAST_PROP];

/**
 * gks_row_get_keyid:
 * @row: #GksRow
 *
 * Returns: text of key id label.
 */
const gchar *
gks_row_get_keyid (GksRow *row)
{
  g_return_val_if_fail (GKS_IS_ROW (row), NULL);

  return gtk_label_get_text (GTK_LABEL (row->priv->name_label));
}

/**
 * gks_row_set_keyid:
 * @row: #GksRow
 * @keyid: text to set for key id label
 *
 * Returns: nothing.
 */
void
gks_row_set_keyid (GksRow      *row,
                   const gchar *keyid)
{
  g_return_if_fail (GKS_IS_ROW (row));

  gtk_label_set_text (GTK_LABEL (row->priv->name_label), keyid);
}

/**
 * gks_row_get_times_signed:
 * @row: #GksRow
 *
 * Returns: times signed.
 */
gint
gks_row_get_times_signed (GksRow *row)
{
  g_return_val_if_fail (GKS_IS_ROW (row), NULL);
  const gchar *tmp = gtk_label_get_text (
      GTK_LABEL (row->priv->times_signed_label));

  return (gint) g_ascii_strtoll (tmp, NULL, 0);
}

/**
 * gks_row_set_times_signed:
 * @row: #GksRow
 * @keyid: text to set for key id label
 *
 * Returns: nothing.
 */
void
gks_row_set_times_signed (GksRow *row,
                          gint    times_signed)
{
  g_return_if_fail (GKS_IS_ROW (row));

  gtk_label_set_text (GTK_LABEL (row->priv->times_signed_label),
                      g_strdup_printf ("%u", times_signed));
}

/**
 * gks_row_get_description:
 * @row: #GksRow
 *
 * Returns: description.
 */
const gchar *
gks_row_get_description (GksRow *row)
{
  g_return_val_if_fail (GKS_IS_ROW (row), NULL);

  return gtk_label_get_text (GTK_LABEL (row->priv->description_label));
}

/**
 * gks_row_set_description:
 * @row: #GksRow
 * @keyid: text to set for description label
 *
 * Returns: nothing.
 */
void
gks_row_set_description (GksRow      *row,
                         const gchar *description)
{
  g_return_if_fail (GKS_IS_ROW (row));

  gtk_label_set_text (GTK_LABEL (row->priv->description_label),
                      description);
}

/**
 * gks_row_get_expiration:
 * @row: #GksRow
 *
 * Returns: date of expiration in YYYY-MM-DD format or %NULL.
 */
const gchar *
gks_row_get_expiration (GksRow *row)
{
  g_return_val_if_fail (GKS_IS_ROW (row), NULL);
  gchar **date = g_strsplit (
      gtk_label_get_text (GTK_LABEL (row->priv->expiration_label)),
      " ", 2);
  if (g_strcmp0 (date[1], "never") != 0)
    return date[1];

  return NULL;
}

/**
 * gks_row_set_expiration:
 * @row: #GksRow
 * @ts: string
 *
 * Returns: nothing.
 */
void
gks_row_set_expiration (GksRow      *row,
                        const gchar *date)
{
  g_return_if_fail (GKS_IS_ROW (row));
  gchar *markup;
  const gchar *format = "<span size=\"small\">Expires %s</span>";

  markup = g_markup_printf_escaped (format, date);
  gtk_label_set_markup (GTK_LABEL (row->priv->expiration_label),
                        markup);
  g_free (markup);
}

GtkWidget *
gks_row_new (const gchar *keyid,
             gint         times_signed,
             const gchar *description,
             const gchar *expiration)
{
  return g_object_new (GKS_TYPE_ROW,
                       "keyid", keyid,
                       "times_signed", times_signed,
                       "description", description,
                       "expiration", expiration,
                       NULL);
}

static void
gks_row_finalize (GObject *object)
{
  GksRowPrivate *priv = GKS_ROW (object)->priv;

  G_OBJECT_CLASS (gks_row_parent_class)->finalize (object);
}

static void
gks_row_get_property (GObject    *object,
                      guint       prop_id,
                      GValue     *value,
                      GParamSpec *pspec)
{
  GksRow *self = GKS_ROW (object);

  switch (prop_id)
    {
    case PROP_KEYID:
      g_value_set_string (value, gks_row_get_keyid (self));
      break;
    case PROP_DESCRIPTION:
      g_value_set_string (value, gks_row_get_description (self));
      break;
    case PROP_TIMES_SIGNED:
      g_value_set_int (value, gks_row_get_times_signed (self));
      break;
    case PROP_EXPIRATION:
      g_value_set_string (value, gks_row_get_expiration (self));
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
gks_row_set_property (GObject      *object,
                      guint         prop_id,
                      const GValue *value,
                      GParamSpec   *pspec)
{
  GksRow *self = GKS_ROW (object);

  switch (prop_id)
    {
    case PROP_KEYID:
      gks_row_set_keyid (self, g_value_get_string (value));
      break;
    case PROP_DESCRIPTION:
      gks_row_set_description (self, g_value_get_string (value));
      break;
    case PROP_TIMES_SIGNED:
      gks_row_set_times_signed (self, g_value_get_int (value));
      break;
    case PROP_EXPIRATION:
      gks_row_set_expiration (self, g_value_get_string (value));
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
gks_row_class_init (GksRowClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  object_class->finalize = gks_row_finalize;
  object_class->get_property = gks_row_get_property;
  object_class->set_property = gks_row_set_property;

  gParamSpecs [PROP_KEYID] =
      g_param_spec_string ("keyid", "KeyID", "Key ID",
                           NULL, G_PARAM_READWRITE);
  g_object_class_install_property (object_class, PROP_KEYID,
                                   gParamSpecs [PROP_KEYID]);
  gParamSpecs [PROP_DESCRIPTION] =
      g_param_spec_string ("description", "Description", "Description",
                           NULL, G_PARAM_READWRITE);
  g_object_class_install_property (object_class, PROP_DESCRIPTION,
                                   gParamSpecs [PROP_DESCRIPTION]);
  gParamSpecs [PROP_TIMES_SIGNED] =
      g_param_spec_int ("times-signed", "Times-Signed", "Times Signed",
                        0, G_MAXINT, 0, G_PARAM_READWRITE);
  g_object_class_install_property (object_class, PROP_TIMES_SIGNED,
                                   gParamSpecs [PROP_TIMES_SIGNED]);
  gParamSpecs [PROP_EXPIRATION] =
      g_param_spec_string ("expiration", "Expiration", "Expiration",
                           NULL, G_PARAM_READWRITE);
  g_object_class_install_property (object_class, PROP_EXPIRATION,
                                   gParamSpecs [PROP_EXPIRATION]);
  gtk_widget_class_set_template_from_resource (widget_class,
                                               "/org/gnome/KeySign/gks-row.ui");
  gtk_widget_class_bind_template_child_private (widget_class, GksRow,
                                                name_label);
  gtk_widget_class_bind_template_child_private (widget_class, GksRow,
                                                times_signed_label);
  gtk_widget_class_bind_template_child_private (widget_class, GksRow,
                                                description_label);
  gtk_widget_class_bind_template_child_private (widget_class, GksRow,
                                                expiration_label);
}

static void
gks_row_init (GksRow *self)
{
  self->priv = gks_row_get_instance_private (self);
  gtk_widget_init_template (GTK_WIDGET (self));
}
