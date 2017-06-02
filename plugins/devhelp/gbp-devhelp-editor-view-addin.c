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

#include <devhelp/devhelp.h>
#include <libxml/xmlreader.h>

#include "gbp-devhelp-editor-view-addin.h"
#include "gbp-devhelp-panel.h"

struct _GbpDevhelpEditorViewAddin
{
  GObject         parent_instance;
  IdeEditorView  *editor_view;
  DhBookManager  *books;

  gchar          *previous_text;
  gchar          *tooltip_text;
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

  pane = pnl_dock_bin_get_right_edge (PNL_DOCK_BIN (layout));
  panel = ide_widget_find_child_typed (pane, GBP_TYPE_DEVHELP_PANEL);
  gbp_devhelp_panel_focus_search (GBP_DEVHELP_PANEL (panel), word);
}

static gboolean
query_tooltip_cb (GbpDevhelpEditorViewAddin *self,
                  gint                       x,
                  gint                       y,
                  gboolean                   keyboard_tooltip,
                  GtkTooltip                *tooltip,
                  GtkWidget                 *widget)
{
  GtkTextBuffer *buffer;
  IdeSourceView *source_view;
  GtkSourceLanguage *lang;
  GtkTextIter begin;
  GtkTextIter end;
  DhKeywordModel *keyword_model;
  DhLink *link;
  gchar *selected_text = NULL;
  gchar *uri = NULL;
  gchar *book_name = NULL;

  g_assert (GTK_IS_WIDGET (widget));
  g_assert (GBP_IS_DEVHELP_EDITOR_VIEW_ADDIN (self));
  g_assert (GTK_IS_TOOLTIP (tooltip));

  source_view = ide_editor_view_get_active_source_view (self->editor_view);
  if (source_view == NULL || !GTK_SOURCE_IS_VIEW (source_view))
    return FALSE;

  buffer = GTK_TEXT_BUFFER (ide_editor_view_get_document (self->editor_view));
  if (buffer == NULL)
    return FALSE;

  lang = gtk_source_buffer_get_language (GTK_SOURCE_BUFFER (buffer));
  if (!ide_str_equal0 (gtk_source_language_get_id(lang), "c"))
    return FALSE;

  gtk_text_view_window_to_buffer_coords (GTK_TEXT_VIEW (source_view), GTK_TEXT_WINDOW_WIDGET, x, y, &x, &y);
  gtk_text_view_get_iter_at_location (GTK_TEXT_VIEW (source_view), &begin, x, y);
  gtk_text_view_get_iter_at_location (GTK_TEXT_VIEW (source_view), &end, x, y);

  while (g_unichar_isalpha(gtk_text_iter_get_char (&begin)) || gtk_text_iter_get_char (&begin) == '_')
    gtk_text_iter_backward_char (&begin);
  gtk_text_iter_forward_char (&begin);

  while (g_unichar_isalpha(gtk_text_iter_get_char (&end)) || gtk_text_iter_get_char (&end) == '_')
    gtk_text_iter_forward_char (&end);

  selected_text = gtk_text_buffer_get_text (buffer, &begin, &end, FALSE);
  if (selected_text == NULL)
    return FALSE;

  if (self->previous_text != selected_text)
    {
      keyword_model = dh_keyword_model_new ();
      link = dh_keyword_model_filter (keyword_model, selected_text, NULL, NULL);
      if (link == NULL)
        return FALSE;

      uri = dh_link_get_uri (link);
      book_name = dh_link_get_book_name (link);
      self->tooltip_text = g_strdup_printf ("%s >> %s", book_name, selected_text);
    }
  else
    self->previous_text = selected_text;

  gtk_tooltip_set_text (tooltip, self->tooltip_text);

  return TRUE;

}

/* static void */
/* gbp_devhelp_view_addin_action (GSimpleAction *action, */
/*                                GVariant      *variant, */
/*                                gpointer       user_data) */
/* { */
/*   IdeSourceView *source_view; */
/*   xmlTextReaderPtr reader; */
/*   gchar *selected_text = NULL; */
/*   gchar *file_name = NULL; */
/*   gchar *function_name = NULL; */
/*   gint ret; */

/*   g_print ("Book name: %s\n", dh_link_get_book_name (link)); */
/*   g_print ("File name: %s\n", dh_link_get_file_name (link)); */
/*   g_print ("Uri: %s\n", dh_link_get_uri (link)); */


  /* reader = xmlReaderForFile(file_name, NULL, 0); */
  /* if (reader != NULL) { */
  /*   ret = xmlTextReaderRead(reader); */
  /*   while (ret == 1) */
  /*     { */
  /*       if (xmlTextReaderConstName(reader) != NULL) */
  /*         { */
  /*           g_print("%d %d %s %d %d\n", */
	 /*            xmlTextReaderDepth(reader), */
	 /*            xmlTextReaderNodeType(reader), */
	 /*            xmlTextReaderConstName(reader), */
	 /*            xmlTextReaderIsEmptyElement(reader), */
	 /*            xmlTextReaderHasValue(reader)); */
  /*         } */
  /*       ret = xmlTextReaderRead(reader); */
  /*     } */
  /*     xmlFreeTextReader(reader); */
  /*     if (ret != 0) */
  /*       g_print ("%s : failed to parse\n", file_name); */
  /* } */
/* } */

static void
gbp_devhelp_editor_view_addin_load (IdeEditorViewAddin *addin,
                                    IdeEditorView      *view)
{
  GbpDevhelpEditorViewAddin *self;

  g_assert (GBP_IS_DEVHELP_EDITOR_VIEW_ADDIN (addin));
  g_assert (IDE_IS_EDITOR_VIEW (view));

  self = GBP_DEVHELP_EDITOR_VIEW_ADDIN (addin);
  self->editor_view = view;
  self->books = dh_book_manager_get_singleton ();

  g_signal_connect_object (view,
                           "request-documentation",
                           G_CALLBACK (request_documentation_cb),
                           addin,
                           G_CONNECT_SWAPPED);

  g_signal_connect_object (view,
                           "query-tooltip",
                           G_CALLBACK (query_tooltip_cb),
                           addin,
                           G_CONNECT_SWAPPED);

  gtk_widget_set_has_tooltip (GTK_WIDGET (view), TRUE);
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
