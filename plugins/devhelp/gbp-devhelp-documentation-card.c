/* gbp-devhelp-documentation-card.c
 *
 * Copyright (C) 2017 Lucie Charvat <luci.charvat@gmail.com>
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

#define G_LOG_DOMAIN "gbp-devhelp-documentation-card"

#include <glib/gi18n.h>
#include <ide.h>
#include <webkit2/webkit2.h>

#include "gbp-devhelp-documentation-card.h"
#include "gbp-devhelp-workbench-addin.h"

struct _GbpDevhelpDocumentationCard
{
  GtkPopover    parent_instance;
  GCancellable *cancellable;

  GtkButton    *button;
  GtkEntry     *entry;
  GtkLabel     *message;
  GtkLabel     *title;
};

G_DEFINE_TYPE (GbpDevhelpDocumentationCard, gbp_devhelp_documentation_card, GTK_TYPE_POPOVER)

enum {
  CREATE_FILE,
  LAST_SIGNAL
};

static guint       signals [LAST_SIGNAL];

static void
gbp_devhelp_documentation_card__button_clicked (GbpDevhelpDocumentationCard *self,
                                     GtkButton        *button)
{
  g_autoptr(GFile) file = NULL;
  const gchar *path;

  g_assert (GBP_IS_DEVHELP_DOCUMENTATION_CARD (self));
  g_assert (GTK_IS_BUTTON (button));

  //if (self->directory == NULL)
  //  return;

  path = gtk_entry_get_text (self->entry);
  if (ide_str_empty0 (path))
    return;

  //file = g_file_get_child (self->directory, path);

  //g_signal_emit (self, signals [CREATE_FILE], 0, file, self->file_type);
}

/* static void */
/* gbp_devhelp_documentation_card__entry_activate (GbpDevhelpDocumentationCard *self, */
/*                                      GtkEntry         *entry) */
/* { */
/*   g_assert (GBP_IS_DEVHELP_DOCUMENTATION_CARD (self)); */
/*   g_assert (GTK_IS_ENTRY (entry)); */

/*   if (gtk_widget_get_sensitive (GTK_WIDGET (self->button))) */
/*     gtk_widget_activate (GTK_WIDGET (self->button)); */
/* } */


/* static void */
/* gbp_devhelp_documentation_card__entry_changed (GbpDevhelpDocumentationCard *self, */
/*                                     GtkEntry         *entry) */
/* { */
/*   const gchar *text; */

/*   g_assert (GBP_IS_DEVHELP_DOCUMENTATION_CARD (self)); */
/*   g_assert (GTK_IS_ENTRY (entry)); */

/*   text = gtk_entry_get_text (entry); */

/*   gtk_widget_set_sensitive (GTK_WIDGET (self->button), !ide_str_empty0 (text)); */

/*  // gbp_devhelp_documentation_card_check_exists (self, self->directory, text); */
/* } */

/* static void */
/* gbp_devhelp_documentation_card_finalize (GObject *object) */
/* { */
/*   GbpDevhelpDocumentationCard *self = (GbpDevhelpDocumentationCard *)object; */

/*   if (self->cancellable && !g_cancellable_is_cancelled (self->cancellable)) */
/*     g_cancellable_cancel (self->cancellable); */

/*   g_clear_object (&self->cancellable); */
/*   //g_clear_object (&self->directory); */

/*   G_OBJECT_CLASS (gbp_devhelp_documentation_card_parent_class)->finalize (object); */
/* } */

static void
gbp_devhelp_documentation_card_class_init (GbpDevhelpDocumentationCardClass *klass)
{
  //GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  /* object_class->finalize = gbp_devhelp_documentation_card_finalize; */
  /* signals [CREATE_FILE] = */
  /*   g_signal_new ("create-file", */
  /*                 G_TYPE_FROM_CLASS (klass), */
  /*                 G_SIGNAL_RUN_FIRST, */
  /*                 0, */
  /*                 NULL, NULL, NULL, */
  /*                 G_TYPE_NONE, */
  /*                 2, */
  /*                 G_TYPE_FILE, */
  /*                 G_TYPE_FILE_TYPE); */

  gtk_widget_class_set_template_from_resource (widget_class, "/org/gnome/builder/plugins/devhelp-plugin/gbp-devhelp-documentation-card.ui");
  //gtk_widget_class_bind_template_child (widget_class, GbpDevhelpDocumentationCard, button);
  gtk_widget_class_bind_template_child (widget_class, GbpDevhelpDocumentationCard, message);
  gtk_widget_class_bind_template_child (widget_class, GbpDevhelpDocumentationCard, title);
}

static void
gbp_devhelp_documentation_card_init (GbpDevhelpDocumentationCard *self)
{
  //self->file_type = G_FILE_TYPE_REGULAR;

  gtk_widget_init_template (GTK_WIDGET (self));

  /* g_signal_connect_object (self->entry, */
  /*                          "activate", */
  /*                          G_CALLBACK (gbp_devhelp_documentation_card__entry_activate), */
  /*                          self, */
  /*                          G_CONNECT_SWAPPED); */

  /* g_signal_connect_object (self->entry, */
  /*                          "changed", */
  /*                          G_CALLBACK (gbp_devhelp_documentation_card__entry_changed), */
  /*                          self, */
  /*                          G_CONNECT_SWAPPED); */

  /* g_signal_connect_object (self->button, */
  /*                          "clicked", */
  /*                          G_CALLBACK (gbp_devhelp_documentation_card__button_clicked), */
  /*                          self, */
  /*                          G_CONNECT_SWAPPED); */
}

void
gbp_devhelp_documentation_card_set_text (GbpDevhelpDocumentationCard *self,
                                         gchar                       *title,
                                         gchar                       *text)
{
  gtk_label_set_label (self->title, title);
  gtk_label_set_label (self->message, text);
}

