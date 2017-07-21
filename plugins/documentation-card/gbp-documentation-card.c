/* gbp-documentation-card.c
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

#define G_LOG_DOMAIN "gbp-documentation-card"

#include <glib/gi18n.h>
#include <ide.h>

#include "gbp-documentation-card.h"

#define HOVER_TIMEOUT          1000
#define CARD_WIDTH             80

struct _GbpDocumentationCard
{
  GtkPopover    parent_instance;

  IdeEditorView  *view;
  GdkWindow      *window;
  GdkDevice      *pointer;

  GtkButton    *button;
  //GtkButton    *goto_button;
  GtkEntry     *entry;
  GtkLabel     *header;
  GtkLabel     *text;

  gchar        *uri;
  guint         timeout_id;
  gint          last_x;
  gint          last_y;
};

G_DEFINE_TYPE (GbpDocumentationCard, gbp_documentation_card, GTK_TYPE_POPOVER)

static gboolean
card_popup (gpointer data)
{
  GbpDocumentationCard *self = GBP_DOCUMENTATION_CARD (data);
  GdkRectangle rec = {1, 1, 1, 1};
  gint x, y;

  if (self->timeout_id)
    {
      g_source_remove (self->timeout_id);
      self->timeout_id = 0;
    }

  gdk_window_get_device_position (self->window, self->pointer, &x, &y, NULL);
  if (x == self->last_x && y == self->last_y)
    {
      rec.x = x;
      rec.y = y;
      gtk_popover_set_pointing_to (GTK_POPOVER (self), &rec);
      gtk_popover_popup (GTK_POPOVER (self));
    }

  return FALSE;
}

static gboolean
card_popdown (gpointer data)
{
  GbpDocumentationCard *self = GBP_DOCUMENTATION_CARD (data);

  gtk_popover_popdown (GTK_POPOVER (self));
  gtk_popover_set_modal (GTK_POPOVER (self), FALSE);

  gtk_widget_set_visible (GTK_WIDGET (self->text), FALSE);
  //gtk_widget_set_visible (GTK_WIDGET (self->goto_button), FALSE);
  gtk_widget_set_visible (GTK_WIDGET (self->button), TRUE);

  return FALSE;
}

/* static void */
/* gbp_documentation_card__goto_button_clicked (GbpDocumentationCard *self, */
/*                                              GtkButton            *goto_button) */
/* { */
/*   GbpDevhelpView *view = NULL; */
/*   IdePerspective *perspective; */
/*   IdeWorkbench *workbench; */

/*   g_assert (GBP_IS_DEVHELP_DOCUMENTATION_CARD (self)); */
/*   g_assert (GTK_IS_BUTTON (goto_button)); */

/*   workbench = ide_widget_get_workbench (GTK_WIDGET (self)); */
/*   g_assert (IDE_IS_WORKBENCH (workbench)); */

/*   perspective = ide_workbench_get_perspective_by_name (workbench, "editor"); */
/*   g_assert (IDE_IS_EDITOR_PERSPECTIVE (perspective)); */

/*   view = g_object_new (GBP_TYPE_DEVHELP_VIEW, */
/*                       "visible", TRUE, */
/*                        NULL); */
/*   gtk_container_add (GTK_CONTAINER (perspective), GTK_WIDGET (view)); */

/*   gbp_devhelp_view_set_uri (view, self->uri); */
/*   ide_workbench_focus (workbench, GTK_WIDGET (view)); */

/* } */

static void
gbp_documentation_card__button_clicked (GbpDocumentationCard *self,
                                        GtkButton                   *button)
{
  g_assert (GBP_IS_DOCUMENTATION_CARD (self));
  g_assert (GTK_IS_BUTTON (button));

  gtk_popover_set_modal (GTK_POPOVER (self), TRUE);
  gtk_label_set_width_chars (self->text, CARD_WIDTH);

  gtk_widget_set_visible (GTK_WIDGET (self->text), TRUE);
  //gtk_widget_set_visible (GTK_WIDGET (self->goto_button), TRUE);
  gtk_widget_set_visible (GTK_WIDGET (self->button), FALSE);
}

static void
gbp_documentation_card_class_init (GbpDocumentationCardClass *klass)
{
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  gtk_widget_class_set_template_from_resource (widget_class, "/org/gnome/builder/plugins/documentation-card/gbp-documentation-card.ui");
  gtk_widget_class_bind_template_child (widget_class, GbpDocumentationCard, button);
  // gtk_widget_class_bind_template_child (widget_class, GbpDocumentationCard, goto_button);
  gtk_widget_class_bind_template_child (widget_class, GbpDocumentationCard, header);
  gtk_widget_class_bind_template_child (widget_class, GbpDocumentationCard, text);

}

static void
gbp_documentation_card_init (GbpDocumentationCard *self)
{

  gtk_widget_init_template (GTK_WIDGET (self));

  self->timeout_id = 0;

  g_signal_connect_object (self->button,
                           "clicked",
                           G_CALLBACK (gbp_documentation_card__button_clicked),
                           self,
                           G_CONNECT_SWAPPED);

  /* g_signal_connect_object (self->goto_button, */
  /*                          "clicked", */
  /*                          G_CALLBACK (gbp_documentation_card__goto_button_clicked), */
  /*                          self, */
  /*                          G_CONNECT_SWAPPED); */
}

void
gbp_documentation_card_set_text (GbpDocumentationCard *self,
                                 IdeDocumentationInfo *info)
{
  IdeDocumentationInfoCard *card;

  card = (g_list_first (info->proposals))->data;

  gtk_label_set_markup (self->text, card->text);
  gtk_label_set_markup (self->header, card->header);
  //gtk_button_set_label (self->goto_button, info->book_name);
}

void
gbp_documentation_card_popup (GbpDocumentationCard *self)
{
  GdkDisplay *display;

  g_assert (GBP_IS_DOCUMENTATION_CARD (self));

  self->window = gtk_widget_get_parent_window (gtk_popover_get_relative_to (GTK_POPOVER (self)));
  display = gdk_window_get_display (self->window);
  self->pointer = gdk_seat_get_pointer (gdk_display_get_default_seat (display));
  gdk_window_get_device_position (self->window, self->pointer, &self->last_x, &self->last_y, NULL);

  if (self->timeout_id)
    g_source_remove (self->timeout_id);

  self->timeout_id = gdk_threads_add_timeout (HOVER_TIMEOUT,
					                                    card_popup,
					                                    g_object_ref (self));
}

void
gbp_documentation_card_popdown (GbpDocumentationCard *self)
{
  card_popdown (g_object_ref (self));
}

