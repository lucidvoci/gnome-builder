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

#define HOVER_TIMEOUT          1000
#define MAX_WIDTH              100

struct _GbpDevhelpDocumentationCard
{
  GtkPopover    parent_instance;

  IdeEditorView  *view;
  GdkWindow      *window;
  GdkDevice      *pointer;

  GtkButton    *button;
  GtkEntry     *entry;
  GtkLabel     *header;
  GtkLabel     *text;

  guint         timeout_id;
  gint          last_x;
  gint          last_y;
};

G_DEFINE_TYPE (GbpDevhelpDocumentationCard, gbp_devhelp_documentation_card, GTK_TYPE_POPOVER)

static gchar *
remove_tags (gchar *source_html)
{
  GString *text = g_string_new (NULL);
  gboolean in_tag = FALSE;
  gboolean in_bracket = FALSE;
  gboolean white_sym = FALSE;
  guint width = 0;

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
      else if (ch == '[')
        in_bracket = TRUE;
      else if (ch == ']')
        in_bracket = FALSE;
      else if (ch == '@')
        {
          g_string_append_unichar (text, '\n');
          width = 0;
        }
      else if (g_unichar_isspace (ch) && !white_sym)
        white_sym = TRUE;
      else if (!in_tag && !in_bracket && !g_unichar_isspace (ch))
        {
          width++;
          if (white_sym && ch != ',')
            {
              if (width >= MAX_WIDTH)
                {
                  g_string_append_unichar (text, '\n');
                  width = 0;
                }
              else
                g_string_append_unichar (text, ' ');
            }
          white_sym = FALSE;
          g_string_append_unichar (text, ch);
        }
      source_html = g_utf8_next_char (source_html);
    }

  return g_string_free (text, FALSE);
}

static void
xml_parse (GbpDevhelpDocumentationCard *self,
           gchar *file_name,
           gchar *func_name)
{
  GFileInputStream *file_stream;
  GDataInputStream *data_stream;
  GString *header = g_string_new (NULL);
  GString *text = g_string_new (NULL);
  GError *error = NULL;
  GMatchInfo *match_info;
  GRegex *regex;
  GRegex *regex_start_header;
  GRegex *regex_end_header;
  GRegex *regex_start;
  GRegex *regex_end;
  GRegex *new_line;
  GRegex *new_point;
  const gchar *regex_char;
  gchar *line;
  gchar *line_tmp;
  gboolean found_tag = FALSE;
  gboolean start_tag = FALSE;
  gboolean text_tag = FALSE;
  gint div_num = 1;

  file_stream = g_file_read (g_file_new_for_uri (file_name), NULL, &error);
  if (file_stream == NULL)
    return;

  regex_char = g_strdup_printf ("name=\"%s\"", func_name);
  regex = g_regex_new (regex_char, 0, 0, NULL);
  regex_start_header = g_regex_new (".*<pre.*?>", 0, 0, NULL);
  regex_end_header = g_regex_new ("</pre.*", 0, 0, NULL);
  regex_start = g_regex_new (".*<div.*?>", 0, 0, NULL);
  regex_end = g_regex_new ("</div.*", 0, 0, NULL);
  new_line = g_regex_new ("<h[0-9]>", 0, 0, NULL);
  new_point = g_regex_new ("<tr>", 0, 0, NULL);

  data_stream = g_data_input_stream_new (G_INPUT_STREAM (file_stream));

  while (TRUE)
    {
      line = g_data_input_stream_read_line_utf8 (data_stream, NULL, NULL, &error);
      if (error != NULL || line == NULL)
        return;
      if (!found_tag)
        {
	        g_regex_match (regex, line, 0, &match_info);
          if (g_match_info_matches (match_info))
            found_tag = TRUE;
        }
      if (found_tag && !start_tag)
        {
          g_regex_match (regex_start_header, line, 0, &match_info);
          if (g_match_info_matches (match_info))
            {
              start_tag = TRUE;
              line_tmp = g_regex_replace (regex_start_header, line, -1, 0, "", 0, NULL);
              g_free (line);
              line = line_tmp;
            }
        }
      if (found_tag && start_tag && !text_tag)
        {
          line_tmp = g_regex_replace (new_line, line, -1, 0, "@@", 0, NULL);
          g_free (line);
          line = line_tmp;
          line_tmp = g_regex_replace (new_point, line, -1, 0, "@", 0, NULL);
          g_free (line);
          line = line_tmp;

          g_regex_match (regex_end_header, line, 0, &match_info);
          if (g_match_info_matches (match_info))
            {
              line_tmp = g_regex_replace (regex_end_header, line, -1, 0, "", 0, NULL);
              g_string_append (header, line_tmp);
              g_free (line_tmp);
              text_tag = TRUE;
              continue;
            }
          g_string_append (header, line);
        }
      if (text_tag)
        {
          line_tmp = g_regex_replace (new_line, line, -1, 0, "@@", 0, NULL);
          g_free (line);
          line = line_tmp;
          line_tmp = g_regex_replace (new_point, line, -1, 0, "@", 0, NULL);
          g_free (line);
          line = line_tmp;

          g_regex_match (regex_start, line, 0, &match_info);
          if (g_match_info_matches (match_info))
            div_num++;
          g_regex_match (regex_end, line, 0, &match_info);
          if (g_match_info_matches (match_info))
            {
              div_num--;
              if (div_num == 0)
                {
                  line_tmp = g_regex_replace (regex_end, line, -1, 0, "", 0, NULL);
                  g_string_append (text, line_tmp);
                  break;
                }
            }
          g_string_append (text, line);
        }
    }

  gtk_label_set_label (self->header, remove_tags (g_string_free (header, FALSE)));
  gtk_label_set_label (self->text, remove_tags (g_string_free (text, FALSE)));

  return;
}

static gboolean
card_popup (gpointer data)
{
  GbpDevhelpDocumentationCard *self = GBP_DEVHELP_DOCUMENTATION_CARD (data);
  GdkRectangle rec = {1, 1, 1, 1};
  gint x, y;

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
  gtk_widget_set_visible (GTK_WIDGET (self->text), FALSE);
  gtk_widget_set_visible (GTK_WIDGET (self->button), TRUE);

  return FALSE;
}

static void
gbp_devhelp_documentation_card__button_clicked (GbpDevhelpDocumentationCard *self,
                                                GtkButton        *button)
{
  g_assert (GBP_IS_DEVHELP_DOCUMENTATION_CARD (self));
  g_assert (GTK_IS_BUTTON (button));

  gtk_widget_set_visible (GTK_WIDGET (self->text), TRUE);
  gtk_widget_set_visible (GTK_WIDGET (self->button), FALSE);

}

static void
gbp_devhelp_documentation_card_class_init (GbpDevhelpDocumentationCardClass *klass)
{
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  gtk_widget_class_set_template_from_resource (widget_class, "/org/gnome/builder/plugins/devhelp-plugin/gbp-devhelp-documentation-card.ui");
  gtk_widget_class_bind_template_child (widget_class, GbpDevhelpDocumentationCard, button);
  gtk_widget_class_bind_template_child (widget_class, GbpDevhelpDocumentationCard, header);
  gtk_widget_class_bind_template_child (widget_class, GbpDevhelpDocumentationCard, text);

}

static void
gbp_devhelp_documentation_card_init (GbpDevhelpDocumentationCard *self)
{

  gtk_widget_init_template (GTK_WIDGET (self));

  g_signal_connect_object (self->button,
                           "clicked",
                           G_CALLBACK (gbp_devhelp_documentation_card__button_clicked),
                           self,
                           G_CONNECT_SWAPPED);
}

void
gbp_devhelp_documentation_card_set_text (GbpDevhelpDocumentationCard *self,
                                         gchar                       *file_name,
                                         gchar                       *func_name)
{
  xml_parse (self, file_name, func_name);
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

  self->timeout_id = gdk_threads_add_timeout (HOVER_TIMEOUT,
					                                    card_popup,
					                                    g_object_ref (self));
}

void
gbp_devhelp_documentation_card_popdown (GbpDevhelpDocumentationCard *self)
{
  card_popdown (g_object_ref (self));
}

