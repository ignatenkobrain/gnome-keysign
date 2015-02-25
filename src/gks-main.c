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
#include <gpgme.h>
#include <gtk/gtk.h>
#include <locale.h>
#include <stdlib.h>

#include "gks-cleanup.h"
#include "gks-gpg.h"

typedef struct {
  GPtrArray      *keys;
  GPtrArray      *secret_keys;
  GtkApplication *application;
  GtkBuilder     *builder;
} GksPrivate;

static void
gks_remove_childrens (GtkContainer *container)
{
  GList *children, *iter;

  children = gtk_container_get_children (container);
  for (iter = children; iter != NULL; iter = g_list_next(iter))
    gtk_widget_destroy (GTK_WIDGET (iter->data));
  g_list_free (children);
}

static void
gks_add_key_to_list (gpointer data,
                     gpointer user_data)
{
  gpgme_key_t key = (gpgme_key_t) data;
  GtkListBox *lbox = GTK_LIST_BOX (user_data);

  GtkWidget *label = gtk_label_new (key->subkeys->keyid);
  gtk_list_box_insert (lbox, label, -1);
  gtk_widget_show (label);
}
static void
gks_refresh_keys (GApplication *application,
                  GksPrivate   *priv)
{
  GtkListBox *my_keys, *signed_keys;
  GksGpg *gpg = gks_gpg_new ();

  g_ptr_array_unref (priv->keys);
  g_ptr_array_unref (priv->secret_keys);
  priv->keys = gks_gpg_list_keys (gpg, FALSE);
  priv->secret_keys = gks_gpg_list_keys (gpg, TRUE);

  my_keys = GTK_LIST_BOX (gtk_builder_get_object (priv->builder, "my_keys"));
  signed_keys = GTK_LIST_BOX (gtk_builder_get_object (priv->builder, "signed_keys"));

  gks_remove_childrens (GTK_CONTAINER (my_keys));
  g_ptr_array_foreach (priv->secret_keys, (GFunc) gks_add_key_to_list,
                       (gpointer) my_keys);

  gks_remove_childrens (GTK_CONTAINER (signed_keys));
  g_ptr_array_foreach (priv->keys, (GFunc) gks_add_key_to_list,
                       (gpointer) signed_keys);
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
  GtkWidget *main_window, *refresh_btn;
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

  priv->keys = g_ptr_array_new_with_free_func ((GDestroyNotify) gpgme_key_unref);
  priv->secret_keys = g_ptr_array_new_with_free_func ((GDestroyNotify) gpgme_key_unref);

  priv->application = gtk_application_new ("org.gnome.KeySign", 0);
  g_signal_connect (priv->application, "startup",
                    G_CALLBACK (gks_startup_cb), priv);
  g_signal_connect (priv->application, "activate",
                    G_CALLBACK (gks_activate_cb), priv);

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
    g_free (priv);
  }
  return status;
}
