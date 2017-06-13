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

  pane = dzl_dock_bin_get_right_edge (DZL_DOCK_BIN (layout));
  panel = dzl_gtk_widget_find_child_typed (pane, GBP_TYPE_DEVHELP_PANEL);
  gbp_devhelp_panel_focus_search (GBP_DEVHELP_PANEL (panel), word);
}

static gchar *
remove_tags (gchar *source_html)
{
  GString *text = g_string_new (NULL);
  gboolean in_tag = FALSE;
  gboolean white_sym = FALSE;

  while (*source_html)
    {
      gunichar ch = g_utf8_get_char (source_html);
      if (ch == '<')
        in_tag = TRUE;
      else if (ch == '>')
        {
          in_tag = FALSE;
          if (!white_sym)
              white_sym = TRUE;
        }
      else if (g_unichar_isspace (ch) && !white_sym)
          white_sym = TRUE;
      else if (!in_tag && !g_unichar_isspace (ch))
        {
          if (white_sym && ch != ',')
            g_string_append_unichar (text, ' ');
          white_sym = FALSE;
          g_string_append_unichar (text, ch);
        }
      source_html = g_utf8_next_char (source_html);
    }

  return g_string_free (text, FALSE);
}

static gchar *
xml_parse (gchar *file_name,
           gchar *func_name)
{
  GFileInputStream *file_stream;
  GDataInputStream *data_stream;
  GString *source = g_string_new (NULL);
  GError *error = NULL;
  GMatchInfo *match_info;
  GRegex *regex;
  GRegex *regex_start;
  GRegex *regex_end;
  const gchar *regex_char;
  gchar *line;
  gchar *line_start;
  gchar *line_end;
  gboolean found_tag = FALSE;
  gboolean start_tag = FALSE;

  file_stream = g_file_read (g_file_new_for_uri (file_name), NULL, &error);
  if (file_stream == NULL)
    return g_string_free (source, FALSE);

  regex_char = g_strdup_printf ("name=\"%s\"", func_name);
  regex = g_regex_new (regex_char, 0, 0, NULL);
  regex_start = g_regex_new (".*<pre.*?>", 0, 0, NULL);
  regex_end = g_regex_new ("</pre.*", 0, 0, NULL);

  data_stream = g_data_input_stream_new (G_INPUT_STREAM (file_stream));

  while (TRUE)
    {
      line = g_data_input_stream_read_line_utf8 (data_stream, NULL, NULL, &error);
      if (error != NULL || line == NULL)
        return g_string_free (source, FALSE);

      if (!found_tag)
        {
	        g_regex_match (regex, line, 0, &match_info);
          if (g_match_info_matches (match_info))
            found_tag = TRUE;
        }
      if (found_tag && !start_tag)
        {
          g_regex_match (regex_start, line, 0, &match_info);
          if (g_match_info_matches (match_info))
            {
              start_tag = TRUE;
              line_start = g_regex_replace (regex_start, line, -1, 0, "", 0, NULL);
              g_free (line);
              line = line_start;
            }
        }
      if (found_tag && start_tag)
        {
          g_regex_match (regex_end, line, 0, &match_info);
          if (g_match_info_matches (match_info))
            {
              line_end = g_regex_replace (regex_end, line, -1, 0, "", 0, NULL);
              g_string_append (source, line_end);
              break;
            }
          g_string_append (source, line);
        }
    }

  return remove_tags (g_string_free (source, FALSE));
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
  const gchar *selected_text;
  const gchar *uri;
  // const gchar *book_name = NULL;
  gchar **tokens;

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
  if (lang == NULL || !ide_str_equal0 (gtk_source_language_get_id(lang), "c"))
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
  if (selected_text == NULL || g_strcmp0 (selected_text, "") == 0)
    return FALSE;

  if (g_strcmp0 (self->previous_text, selected_text) != 0)
    {
      keyword_model = dh_keyword_model_new ();
      link = dh_keyword_model_filter (keyword_model, selected_text, NULL, NULL);
      if (link == NULL)
        return FALSE;

      uri = dh_link_get_uri (link);
      // book_name = dh_link_get_book_name (link);
      tokens = g_strsplit (uri, "#", -1 );
      if (tokens == NULL)
        {
          return FALSE;
        }

      if (g_strv_length (tokens) != 2)
        {
          g_strfreev (tokens);
          return FALSE;
        }

      g_free (self->tooltip_text);
      self->tooltip_text = xml_parse (tokens[0], tokens[1]);
      g_strfreev (tokens);
      if (g_strcmp0 (self->tooltip_text, "") == 0)
        return FALSE;

      g_free (self->previous_text);
      self->previous_text = g_strdup (selected_text);
    }

  gtk_tooltip_set_text (tooltip, self->tooltip_text);

  return TRUE;

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
