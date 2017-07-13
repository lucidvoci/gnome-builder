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

#include "gbp-devhelp-documentation-card.h"
#include "gbp-devhelp-view.h"

#define HOVER_TIMEOUT          1000
#define CARD_WIDTH             80

struct _GbpDevhelpDocumentationCard
{
  GtkPopover    parent_instance;

  IdeEditorView  *view;
  GdkWindow      *window;
  GdkDevice      *pointer;

  GtkButton    *button;
  GtkButton    *goto_button;
  GtkEntry     *entry;
  GtkLabel     *header;
  GtkLabel     *text;

  gchar        *uri;
  guint         timeout_id;
  gint          last_x;
  gint          last_y;
};

G_DEFINE_TYPE (GbpDevhelpDocumentationCard, gbp_devhelp_documentation_card, GTK_TYPE_POPOVER)

enum
{
  START_HEADER,
  END_HEADER,
  END_TEXT,

  REMOVE_TAG_HEADER,
  REMOVE_TAG_TEXT,
  REMOVE_MULTI_SPACES,
  NEW_LINE,
  NEW_PARAGRAPH,
  MAKE_BOLD_START,
  MAKE_BOLD_END,
  MAKE_BOLD_START_NEW_LINE,
  MAKE_BOLD_END_NEW_LINE,
  MAKE_POINT_NEW_LINE,
  INFORMAL_EXAMPLE,
  INFORMAL_EXAMPLE_END,

  N_REGEXES
};

static GRegex *regexes [N_REGEXES];


static gchar*
regex_replace_line (GRegex      *regex,
                    gchar       *line,
                    gchar       *replace)
{
  gchar *tmp_line;

  tmp_line = g_regex_replace (regex, line, -1, 0, replace, 0, NULL);
  g_free (line);
  return tmp_line;
}

static gboolean
xml_parse (GbpDevhelpDocumentationCard *self,
           gchar                       *file_name,
           gchar                       *func_name,
           const gchar                 *book_name)
{
  GFileInputStream *file_stream;
  GDataInputStream *data_stream;
  GString *header = g_string_new (NULL);
  GString *text = g_string_new (NULL);
  GError *error = NULL;
  GMatchInfo *match_info;
  GRegex *start_text;

  const gchar *regex_char;
  gchar *line;
  gboolean informal_example_bool = FALSE;
  gboolean found_tag = FALSE;
  gboolean text_tag = FALSE;

  file_stream = g_file_read (g_file_new_for_uri (file_name), NULL, &error);
  if (file_stream == NULL)
    return FALSE;

  regex_char = g_strdup_printf ("name=\"%s\"", func_name);
  start_text = g_regex_new (regex_char, 0, 0, NULL);

  data_stream = g_data_input_stream_new (G_INPUT_STREAM (file_stream));

  while (TRUE)
    {
      line = g_data_input_stream_read_line_utf8 (data_stream, NULL, NULL, &error);
      if (error != NULL || line == NULL)
        return FALSE;
      if (!found_tag)
        {
          if (g_regex_match (start_text, line, 0, &match_info))
            found_tag = TRUE;
        }
      if (found_tag && !text_tag)
        {
          line = regex_replace_line (regexes[REMOVE_TAG_HEADER], line, "");
          line = regex_replace_line (regexes[MAKE_BOLD_START], line, "<b>");
          line = regex_replace_line (regexes[MAKE_BOLD_END], line, "</b>");
          line = regex_replace_line (regexes[START_HEADER], line, "<tt>");
          line = regex_replace_line (regexes[NEW_LINE], line, "\n");

          if (g_regex_match (regexes[END_HEADER], line, 0, &match_info))
            {
              line = regex_replace_line (regexes[END_HEADER], line, "</tt>");
              g_string_append (header, line);
              text_tag = TRUE;
              continue;
            }
          g_string_append_printf (header, "%s\n", line);
        }
      if (text_tag)
        {
          if (g_regex_match (regexes[INFORMAL_EXAMPLE], line, 0, &match_info))
            {
              informal_example_bool = TRUE;
              continue;
            }
          if (g_regex_match (regexes[INFORMAL_EXAMPLE_END], line, 0, &match_info))
            {
              informal_example_bool = FALSE;
              continue;
            }
          line = regex_replace_line (regexes[NEW_PARAGRAPH], line, "\t");
          line = regex_replace_line (regexes[REMOVE_TAG_TEXT], line, "");
          line = regex_replace_line (regexes[MAKE_BOLD_START], line, "<b>");
          line = regex_replace_line (regexes[MAKE_BOLD_END], line, "</b>");
          line = regex_replace_line (regexes[MAKE_BOLD_START_NEW_LINE], line, "\n<b>");
          line = regex_replace_line (regexes[MAKE_BOLD_END_NEW_LINE], line, "</b>\n");
          line = regex_replace_line (regexes[MAKE_POINT_NEW_LINE], line, " - ");

          if (g_regex_match (regexes[REMOVE_MULTI_SPACES], line, 0, &match_info))
            continue;

          line = regex_replace_line (regexes[NEW_LINE], line, "\n");

          if (g_regex_match (regexes[END_TEXT], line, 0, &match_info))
            break;

          if (informal_example_bool)
            g_string_append_printf (text, "\n<tt>%s</tt>", line);
          else
            g_string_append_printf (text, "%s ", line);
        }
    }

  gtk_label_set_markup (self->header, g_string_free (header, FALSE));
  gtk_label_set_markup (self->text, g_string_free (text, FALSE));
  gtk_button_set_label (self->goto_button, book_name);

  return TRUE;
}

static gboolean
card_popup (gpointer data)
{
  GbpDevhelpDocumentationCard *self = GBP_DEVHELP_DOCUMENTATION_CARD (data);
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
  GbpDevhelpDocumentationCard *self = GBP_DEVHELP_DOCUMENTATION_CARD (data);

  gtk_popover_popdown (GTK_POPOVER (self));
  gtk_popover_set_modal (GTK_POPOVER (self), FALSE);

  gtk_widget_set_visible (GTK_WIDGET (self->text), FALSE);
  gtk_widget_set_visible (GTK_WIDGET (self->goto_button), FALSE);
  gtk_widget_set_visible (GTK_WIDGET (self->button), TRUE);

  return FALSE;
}

static void
gbp_devhelp_documentation_card__goto_button_clicked (GbpDevhelpDocumentationCard *self,
                                                     GtkButton                   *goto_button)
{
  GbpDevhelpView *view = NULL;
  IdePerspective *perspective;
  IdeWorkbench *workbench;

  g_assert (GBP_IS_DEVHELP_DOCUMENTATION_CARD (self));
  g_assert (GTK_IS_BUTTON (goto_button));

  workbench = ide_widget_get_workbench (GTK_WIDGET (self));
  g_assert (IDE_IS_WORKBENCH (workbench));

  perspective = ide_workbench_get_perspective_by_name (workbench, "editor");
  g_assert (IDE_IS_EDITOR_PERSPECTIVE (perspective));

  view = g_object_new (GBP_TYPE_DEVHELP_VIEW,
                      "visible", TRUE,
                       NULL);
  gtk_container_add (GTK_CONTAINER (perspective), GTK_WIDGET (view));

  gbp_devhelp_view_set_uri (view, self->uri);
  ide_workbench_focus (workbench, GTK_WIDGET (view));

}

static void
gbp_devhelp_documentation_card__button_clicked (GbpDevhelpDocumentationCard *self,
                                                GtkButton                   *button)
{
  g_assert (GBP_IS_DEVHELP_DOCUMENTATION_CARD (self));
  g_assert (GTK_IS_BUTTON (button));

  gtk_popover_set_modal (GTK_POPOVER (self), TRUE);
  gtk_label_set_width_chars (self->text, CARD_WIDTH);

  gtk_widget_set_visible (GTK_WIDGET (self->text), TRUE);
  gtk_widget_set_visible (GTK_WIDGET (self->goto_button), TRUE);
  gtk_widget_set_visible (GTK_WIDGET (self->button), FALSE);
}

static void
gbp_devhelp_documentation_card_class_init (GbpDevhelpDocumentationCardClass *klass)
{
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  gtk_widget_class_set_template_from_resource (widget_class, "/org/gnome/builder/plugins/devhelp-plugin/gbp-devhelp-documentation-card.ui");
  gtk_widget_class_bind_template_child (widget_class, GbpDevhelpDocumentationCard, button);
  gtk_widget_class_bind_template_child (widget_class, GbpDevhelpDocumentationCard, goto_button);
  gtk_widget_class_bind_template_child (widget_class, GbpDevhelpDocumentationCard, header);
  gtk_widget_class_bind_template_child (widget_class, GbpDevhelpDocumentationCard, text);

  regexes[START_HEADER] = g_regex_new (".*<pre.*?>", 0, 0, NULL);
  regexes[END_HEADER] = g_regex_new ("</pre.*", 0, 0, NULL);
  regexes[END_TEXT] = g_regex_new ("<hr>", 0, 0, NULL);

  regexes[REMOVE_TAG_HEADER] = g_regex_new ("</?code.*?>|</?span.*?>|<h3.*/h3>|</?table.*?>|</?col.*?>|</?em.*?>", 0, 0, NULL);
  regexes[REMOVE_TAG_TEXT] = g_regex_new ("<p.*?>|</?pre.*?>|</?div.*>|</li>|</?td.*?>|</?tbody>|</?ul.*?>|</?code.*?>|</?span.*?>|</?table.*?>|</?col.*?>|</?em.*?>|</?acronym.*?>|^\\s*", 0, 0, NULL);
  regexes[REMOVE_MULTI_SPACES] = g_regex_new ("^\\s*$|^[\\d|\\s]*$", 0, 0, NULL);
  regexes[NEW_LINE] = g_regex_new ("</tr>|</p>", 0, 0, NULL);
  regexes[NEW_PARAGRAPH] = g_regex_new ("</p></td>", 0, 0, NULL);
  regexes[MAKE_BOLD_START] = g_regex_new ("<a.*?>", 0, 0, NULL);
  regexes[MAKE_BOLD_END] = g_regex_new ("</a>", 0, 0, NULL);
  regexes[MAKE_BOLD_START_NEW_LINE] = g_regex_new ("<h4.*?>", 0, 0, NULL);
  regexes[MAKE_BOLD_END_NEW_LINE] = g_regex_new ("</h4>", 0, 0, NULL);
  regexes[MAKE_POINT_NEW_LINE] = g_regex_new ("<li.*?>|<tr>", 0, 0, NULL);
  regexes[INFORMAL_EXAMPLE] = g_regex_new ("<div class=\"informalexample\">", 0, 0, NULL);
  regexes[INFORMAL_EXAMPLE_END] = g_regex_new ("</div>", 0, 0, NULL);

}

static void
gbp_devhelp_documentation_card_init (GbpDevhelpDocumentationCard *self)
{

  gtk_widget_init_template (GTK_WIDGET (self));

  self->timeout_id = 0;

  g_signal_connect_object (self->button,
                           "clicked",
                           G_CALLBACK (gbp_devhelp_documentation_card__button_clicked),
                           self,
                           G_CONNECT_SWAPPED);

  g_signal_connect_object (self->goto_button,
                           "clicked",
                           G_CALLBACK (gbp_devhelp_documentation_card__goto_button_clicked),
                           self,
                           G_CONNECT_SWAPPED);
}

gboolean
gbp_devhelp_documentation_card_set_text (GbpDevhelpDocumentationCard *self,
                                         gchar                       *uri,
                                         const gchar                 *book_name)
{
  gboolean parse_succ = FALSE;
  gchar **tokens;

  self->uri = g_strdup (uri);
  tokens = g_strsplit (uri, "#", -1 );
  if (tokens == NULL)
    return FALSE;
  if (g_strv_length (tokens) != 2)
    {
      g_strfreev (tokens);
      return FALSE;
    }

  parse_succ = xml_parse (self, tokens[0], tokens[1], book_name);
  g_strfreev (tokens);

  return parse_succ;
}

void
gbp_devhelp_documentation_card_popup (GbpDevhelpDocumentationCard *self)
{
  GdkDisplay *display;

  g_assert (GBP_IS_DEVHELP_DOCUMENTATION_CARD (self));

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
gbp_devhelp_documentation_card_popdown (GbpDevhelpDocumentationCard *self)
{
  card_popdown (g_object_ref (self));
}

