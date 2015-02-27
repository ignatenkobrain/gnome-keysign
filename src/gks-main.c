/* gks-main.c
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

#include <glib.h>
#include <glib/gstdio.h>
#include <gpgme.h>
#include <gtk/gtk.h>
#include <locale.h>
#include <stdlib.h>

#include "gks-cleanup.h"
#include "gks-gpg.h"
#include "gks-row.h"

typedef struct {
  GPtrArray      *keys;
  GtkApplication *application;
  GtkBuilder     *builder;
  gchar          *cachefile;
  GKeyFile       *data;
} GksPrivate;

typedef struct {
  gpgme_key_t key;
  guint       times_signed;
} GksKey;

static void
gks_key_free (GksKey *key)
{
  gpgme_key_unref (key->key);
}

static GksKey *
gks_key_new (gpgme_key_t key,
             guint       times_signed)
{
  GksKey *k;
  k = g_new0 (GksKey, 1);
  k->times_signed = times_signed;
  k->key = key;
  return k;
}

static void
gks_load_cache (GksPrivate *priv)
{
  g_key_file_load_from_file (priv->data, priv->cachefile,
                             G_KEY_FILE_NONE, NULL);
}

static void
gks_save_cache (GksPrivate *priv)
{
  g_key_file_save_to_file (priv->data, priv->cachefile, NULL);
}

static void
gks_remove_childrens (GtkContainer *container)
{
  GList *children, *iter;

  children = gtk_container_get_children (container);
  for (iter = children; iter != NULL; iter = g_list_next(iter))
    gtk_widget_destroy (GTK_WIDGET (iter->data));
  g_list_free (children);
}

gboolean
gks_is_signed (GksRow   *row,
               gpointer  user_data)
{
  if (gks_row_get_times_signed (row) > 0)
    return TRUE;

  return FALSE;
}

static void
gks_add_key_to_list (GksKey               *k,
                     GtkListBox           *lbox,
                     GtkListBoxFilterFunc  filter)
{
  gpgme_key_t key = k->key;
  gpgme_user_id_t subkey;
  GtkWidget *row;
  gint i;
  gchar *comment, *expires;
  _cleanup_error_free_ GError *error = NULL;
  GDateTime *date;

  subkey = key->uids;
  i = 0;
  comment = g_strdup ("");
  while (subkey != NULL) {
    comment = g_strconcat (comment, subkey->uid, "\n", NULL);
    subkey = subkey->next;
    i++;
  }

  expires = g_strdup ("never");
  if (key->subkeys->expires != 0) {
    date = g_date_time_new_from_unix_local (key->subkeys->expires);
    expires = g_date_time_format (date, "%Y-%m-%d");
    g_date_time_unref (date);
  }

  row = GTK_WIDGET (gks_row_new (key->subkeys->keyid,
                                 k->times_signed,
                                 comment,
                                 expires));

  gtk_list_box_insert (lbox, row, -1);
  gtk_list_box_set_filter_func (lbox, filter, NULL, NULL);
  gtk_widget_show_all (row);
  g_free (comment);
  g_free (expires);
}

static void
gks_refresh_keys_ui (GksPrivate *priv)
{
  GtkListBox *my_keys = GTK_LIST_BOX (
      gtk_builder_get_object (priv->builder, "my_keys"));
  GtkListBox *signed_keys = GTK_LIST_BOX (
      gtk_builder_get_object (priv->builder, "signed_keys"));

  gks_remove_childrens (GTK_CONTAINER (my_keys));
  gks_remove_childrens (GTK_CONTAINER (signed_keys));

  for (gint i = 0; i < priv->keys->len; i++) {
    GksKey *k = g_ptr_array_index (priv->keys, i);
    gks_add_key_to_list (k, my_keys, NULL);
    gks_add_key_to_list (k, signed_keys, (GtkListBoxFilterFunc) gks_is_signed);
  }
}

static void
gks_refresh_keys (GApplication *application,
                  GksPrivate   *priv)
{
  GksGpg *gpg = gks_gpg_new ();
  gint i;

  GPtrArray *keys = gks_gpg_list_keys (gpg);
  if (priv->keys->len != 0)
    g_ptr_array_remove_range (priv->keys, 0, priv->keys->len);
  for (i = 0; i < keys->len; i++) {
    gpgme_key_t key = g_ptr_array_index (keys, i);
    if (!g_key_file_has_group (priv->data, key->subkeys->keyid)) {
      g_key_file_set_integer (priv->data, key->subkeys->keyid,
                              "times_signed", 0);
      gks_save_cache (priv);
    }
    GksKey *k = gks_key_new (key,
        g_key_file_get_integer (priv->data, key->subkeys->keyid,
                                "times_signed", NULL));
    g_ptr_array_add (priv->keys, k);
  }
  g_object_unref (gpg);

  gks_refresh_keys_ui (priv);
}

static void
gks_key_presented_cb (GtkListBox *box,
                      GksRow     *row,
                      GksPrivate *priv)
{
  const gchar *keyid;
  gint times_signed;

  keyid = gks_row_get_keyid (row);
  for (gint i = 0; i < priv->keys->len; i++) {
    GksKey *key = g_ptr_array_index (priv->keys, i);
    if (g_strcmp0 (key->key->subkeys->keyid, keyid) == 0) {
      key->times_signed++;
      times_signed = key->times_signed;
    }
  }
  g_key_file_set_integer (priv->data, keyid,
                          "times_signed", times_signed);
  gks_row_set_times_signed (row, times_signed);
  gks_save_cache (priv);
}

static void
gks_activate_cb (GApplication *application,
                 GksPrivate   *priv)
{
  GtkWindow *window;
  window = GTK_WINDOW (gtk_builder_get_object (priv->builder, "dialog_main"));
  gtk_window_present (window);
}

static void
gks_startup_cb (GApplication *application,
                GksPrivate   *priv)
{
  GtkWidget *main_window, *refresh_btn, *keys;
  gint retval;
  _cleanup_error_free_ GError *error = NULL;

  priv->builder = gtk_builder_new ();
  retval = gtk_builder_add_from_resource (priv->builder,
                                          "/org/gnome/KeySign/gks-main.ui",
                                          &error);
  if (retval == 0) {
    g_warning ("failed to load ui: %s", error->message);
    return;
  }

  main_window = GTK_WIDGET (gtk_builder_get_object (priv->builder, "dialog_main"));
  gtk_application_add_window (priv->application, GTK_WINDOW (main_window));
  gtk_widget_set_size_request (main_window, 640, 480);
  gtk_widget_hide (main_window);

  refresh_btn = GTK_WIDGET (gtk_builder_get_object (priv->builder, "refresh_btn"));
  g_signal_connect (refresh_btn, "clicked",
                    G_CALLBACK (gks_refresh_keys), priv);
  keys = GTK_WIDGET (gtk_builder_get_object (priv->builder, "my_keys"));
  g_signal_connect (keys, "row-activated",
                    G_CALLBACK (gks_key_presented_cb), priv);
  keys = GTK_WIDGET (gtk_builder_get_object (priv->builder, "signed_keys"));
  g_signal_connect (keys, "row-activated",
                    G_CALLBACK (gks_key_presented_cb), priv);
  /* TODO: connect signal for switching GtkStack to gks_refresh_keys_ui */

  gks_refresh_keys (application, priv);

  gtk_widget_show (main_window);
}

int
main (int    argc,
      char **argv)
{
  GksPrivate *priv = NULL;
  GOptionContext *context;
  gboolean verbose = FALSE;
  int status = EXIT_SUCCESS;
  _cleanup_error_free_ GError *error = NULL;
  const GOptionEntry options[] = {
    { "verbose", 'v', 0, G_OPTION_ARG_NONE, &verbose,
      "Show extra debugging information", NULL },
    { NULL}
  };

  setlocale (LC_ALL, "");

  gtk_init (&argc, &argv);

  context = g_option_context_new ("GNOME KeySign");
  g_option_context_add_group (context, gtk_get_option_group (TRUE));
  g_option_context_add_main_entries (context, options, NULL);
  if (!g_option_context_parse (context, &argc, &argv, &error)) {
    status = EXIT_FAILURE;
    g_print ("%s: %s\n", "Failed to parse command line options",
             error->message);
    goto out;
  }

  priv = g_new0 (GksPrivate, 1);

  priv->keys = g_ptr_array_new_with_free_func ((GDestroyNotify) gks_key_free);

  priv->application = gtk_application_new ("org.gnome.KeySign", 0);
  g_signal_connect (priv->application, "startup",
                    G_CALLBACK (gks_startup_cb), priv);
  g_signal_connect (priv->application, "activate",
                    G_CALLBACK (gks_activate_cb), priv);

  priv->cachefile = g_build_filename (g_get_user_data_dir (),
                                      "gnome-keysign", "signs", NULL);
  if (g_mkdir_with_parents (g_path_get_dirname (priv->cachefile), 0755) != 0) {
    status = EXIT_FAILURE;
    g_print ("Failed to create data directory");
    goto out;
  }
  priv->data = g_key_file_new ();
  gks_load_cache (priv);

  if (verbose)
    g_setenv ("G_MESSAGES_DEBUG", "GnomeKeySign", FALSE);

  status = g_application_run (G_APPLICATION (priv->application), argc, argv);
out:
  g_option_context_free (context);
  if (priv != NULL) {
    if (priv->builder != NULL)
      g_object_unref (priv->builder);
    if (priv->application != NULL)
      g_object_unref (priv->application);
    if (priv->cachefile != NULL)
      g_free (priv->cachefile);
    if (priv->data != NULL)
      g_key_file_unref (priv->data);
    g_free (priv);
  }
  return status;
}
