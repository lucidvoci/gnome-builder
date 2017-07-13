/* gbp-devhelp-editor-view-addin.c
 *
 * Copyright (C) 2015 Christian Hergert <chergert@redhat.com>
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

#define G_LOG_DOMAIN "gbp-devhelp-editor-view-addin"

#include <devhelp/devhelp.h>
#include <libxml/xmlreader.h>
#include "gbp-devhelp-documentation-card.h"
#include "gbp-devhelp-editor-view-addin.h"
#include "gbp-devhelp-panel.h"

struct _GbpDevhelpEditorViewAddin
{
  GObject         parent_instance;
  IdeEditorView  *editor_view;
  GbpDevhelpDocumentationCard     *popover;
  DhKeywordModel *keyword_model;

  gchar          *previous_text;
};

static void iface_init (IdeEditorViewAddinInterface *iface);

G_DEFINE_TYPE_EXTENDED (GbpDevhelpEditorViewAddin, gbp_devhelp_editor_view_addin, G_TYPE_OBJECT, 0,
                        G_IMPLEMENT_INTERFACE (IDE_TYPE_EDITOR_VIEW_ADDIN, iface_init))

static void
request_documentation_cb (GbpDevhelpEditorViewAddin *self,
                          const gchar               *word,
                          IdeEditorView             *view)
{
  GtkWidget *layout;
  GtkWidget *panel;
  GtkWidget *pane;

  g_assert (IDE_IS_EDITOR_VIEW (view));
  g_assert (GBP_IS_DEVHELP_EDITOR_VIEW_ADDIN (self));

  layout = gtk_widget_get_ancestor (GTK_WIDGET (view), IDE_TYPE_LAYOUT);
  if (layout == NULL)
    return;

  pane = dzl_dock_bin_get_right_edge (DZL_DOCK_BIN (layout));
  panel = dzl_gtk_widget_find_child_typed (pane, GBP_TYPE_DEVHELP_PANEL);
  gbp_devhelp_panel_focus_search (GBP_DEVHELP_PANEL (panel), word);
}

static gboolean
motion_notify_event_cb (gpointer data)
{
  GbpDevhelpEditorViewAddin *self = GBP_DEVHELP_EDITOR_VIEW_ADDIN  (data);
  GtkTextBuffer *buffer;
  IdeSourceView *source_view;
  GtkSourceLanguage *lang;

  GdkDisplay *display;
  GdkWindow *window;
  GdkDevice *device;

  GtkTextIter begin;
  GtkTextIter end;

  DhLink *link;
  gchar *selected_text;
  gchar *uri;
  const gchar *book_name;
  gint x, y;

  source_view = ide_editor_view_get_active_source_view (self->editor_view);
  if (source_view == NULL || !GTK_SOURCE_IS_VIEW (source_view))
    return FALSE;

  buffer = GTK_TEXT_BUFFER (ide_editor_view_get_document (self->editor_view));
  if (buffer == NULL)
    return FALSE;

  lang = gtk_source_buffer_get_language (GTK_SOURCE_BUFFER (buffer));
  if (lang == NULL || !ide_str_equal0 (gtk_source_language_get_id(lang), "c"))
    return FALSE;

  window = gtk_widget_get_parent_window (GTK_WIDGET (self->editor_view));
  display = gdk_window_get_display (window);
  device = gdk_seat_get_pointer (gdk_display_get_default_seat (display));

  gdk_window_get_device_position (window, device, &x, &y, NULL);
  gtk_text_view_window_to_buffer_coords (GTK_TEXT_VIEW (source_view), GTK_TEXT_WINDOW_WIDGET, x, y, &x, &y);
  gtk_text_view_get_iter_at_location (GTK_TEXT_VIEW (source_view), &end, x, y);
  gtk_text_view_get_iter_at_location (GTK_TEXT_VIEW (source_view), &begin, x, y);

  while (g_unichar_islower (gtk_text_iter_get_char (&begin)) || g_unichar_isdigit(gtk_text_iter_get_char (&begin)) || gtk_text_iter_get_char (&begin) == '_')
    gtk_text_iter_backward_char (&begin);
  gtk_text_iter_forward_char (&begin);

  while (g_unichar_islower (gtk_text_iter_get_char (&end)) || g_unichar_isdigit (gtk_text_iter_get_char (&end)) || gtk_text_iter_get_char (&end) == '_')
    gtk_text_iter_forward_char (&end);

  selected_text = gtk_text_buffer_get_text (buffer, &begin, &end, FALSE);
  if (selected_text == NULL || gtk_text_iter_get_offset (&end) - gtk_text_iter_get_offset (&begin) <= 1)
    {
      self->previous_text = NULL;
      gbp_devhelp_documentation_card_popdown (self->popover);
      return FALSE;
    }
  if (g_strcmp0 (self->previous_text, selected_text) != 0)
    {
      gbp_devhelp_documentation_card_popdown (self->popover);
      link = dh_keyword_model_filter (self->keyword_model, selected_text, NULL, NULL);
      if (link == NULL)
        return FALSE;

      uri = dh_link_get_uri (link);
      book_name = dh_link_get_book_name (link);

      if (!gbp_devhelp_documentation_card_set_text (self->popover, uri, book_name))
        return FALSE;

      g_free (self->previous_text);
      self->previous_text = g_strdup (selected_text);
    }

  gbp_devhelp_documentation_card_popup (self->popover);
  return FALSE;
}

static void
gbp_devhelp_editor_view_addin_load (IdeEditorViewAddin *addin,
                                    IdeEditorView      *view)
{
  GbpDevhelpEditorViewAddin *self;

  g_assert (GBP_IS_DEVHELP_EDITOR_VIEW_ADDIN (addin));
  g_assert (IDE_IS_EDITOR_VIEW (view));

  self = GBP_DEVHELP_EDITOR_VIEW_ADDIN (addin);
  self->editor_view = view;
  self->keyword_model = dh_keyword_model_new ();
  self->popover = g_object_new (GBP_TYPE_DEVHELP_DOCUMENTATION_CARD,
                                "relative-to", GTK_WIDGET (view),
                                "position", GTK_POS_TOP,
                                "modal", FALSE,
                                NULL);

  g_signal_connect_object (view,
                           "request-documentation",
                           G_CALLBACK (request_documentation_cb),
                           addin,
                           G_CONNECT_SWAPPED);

  g_signal_connect_object (view,
                           "motion-notify-event",
                           G_CALLBACK (motion_notify_event_cb),
                           self,
                           G_CONNECT_SWAPPED);

}

static void
gbp_devhelp_editor_view_addin_class_init (GbpDevhelpEditorViewAddinClass *klass)
{
}

static void
gbp_devhelp_editor_view_addin_init (GbpDevhelpEditorViewAddin *self)
{
}

static void
iface_init (IdeEditorViewAddinInterface *iface)
{
  iface->load = gbp_devhelp_editor_view_addin_load;
}
