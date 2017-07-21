/* ide-editor-view-settings.c
 *
 * Copyright (C) 2017 Christian Hergert <chergert@redhat.com>
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

#define G_LOG_DOMAIN "ide-editor-view-settings"

#include "ide-editor-private.h"

static gboolean
get_smart_home_end (GValue   *value,
                    GVariant *variant,
                    gpointer  user_data)
{
  if (g_variant_get_boolean (variant))
    g_value_set_enum (value, GTK_SOURCE_SMART_HOME_END_BEFORE);
  else
    g_value_set_enum (value, GTK_SOURCE_SMART_HOME_END_DISABLED);
  return TRUE;
}

static gboolean
get_wrap_mode (GValue   *value,
               GVariant *variant,
               gpointer  user_data)
{
  if (g_variant_get_boolean (variant))
    g_value_set_enum (value, GTK_WRAP_WORD);
  else
    g_value_set_enum (value, GTK_WRAP_NONE);
  return TRUE;
}

static void
on_keybindings_changed (IdeEditorView *self,
                        const gchar   *key,
                        GSettings     *settings)
{
  IdeSourceView *source_view;

  g_assert (IDE_IS_EDITOR_VIEW (self));
  g_assert (g_strcmp0 (key, "keybindings") == 0);
  g_assert (G_IS_SETTINGS (settings));

  source_view = ide_editor_view_get_view (self);

  g_signal_emit_by_name (source_view,
                         "set-mode",
                         NULL,
                         IDE_SOURCE_VIEW_MODE_TYPE_PERMANENT);
}

void
_ide_editor_view_init_settings (IdeEditorView *self)
{
  IdeSourceView *source_view;
  IdeBuffer *buffer;

  g_assert (IDE_IS_EDITOR_VIEW (self));
  g_assert (self->editor_settings == NULL);
  g_assert (self->insight_settings == NULL);

  source_view = ide_editor_view_get_view (self);
  buffer = ide_editor_view_get_buffer (self);

  self->editor_settings = g_settings_new ("org.gnome.builder.editor");

  g_settings_bind (self->editor_settings, "highlight-current-line",
                   source_view, "highlight-current-line",
                   G_SETTINGS_BIND_GET);

  g_settings_bind (self->editor_settings, "highlight-matching-brackets",
                   buffer, "highlight-matching-brackets",
                   G_SETTINGS_BIND_GET);

  g_settings_bind (self->editor_settings, "show-line-changes",
                   source_view, "show-line-changes",
                   G_SETTINGS_BIND_GET);

  g_settings_bind (self->editor_settings, "show-line-numbers",
                   source_view, "show-line-numbers",
                   G_SETTINGS_BIND_GET);

  g_settings_bind (self->editor_settings, "smart-backspace",
                   source_view, "smart-backspace",
                   G_SETTINGS_BIND_GET);

  g_settings_bind_with_mapping (self->editor_settings, "smart-home-end",
                                source_view, "smart-home-end",
                                G_SETTINGS_BIND_GET,
                                get_smart_home_end, NULL, NULL, NULL);

  g_settings_bind (self->editor_settings, "style-scheme-name",
                   buffer, "style-scheme-name",
                   G_SETTINGS_BIND_GET);

  g_settings_bind (self->editor_settings, "font-name",
                   source_view, "font-name",
                   G_SETTINGS_BIND_GET);

  g_settings_bind (self->editor_settings, "overscroll",
                   source_view, "overscroll",
                   G_SETTINGS_BIND_GET);

  g_settings_bind (self->editor_settings, "scroll-offset",
                   source_view, "scroll-offset",
                   G_SETTINGS_BIND_GET);

  g_settings_bind (self->editor_settings, "show-grid-lines",
                   source_view, "show-grid-lines",
                   G_SETTINGS_BIND_GET);

  g_settings_bind_with_mapping (self->editor_settings, "wrap-text",
                                source_view, "wrap-mode",
                                G_SETTINGS_BIND_GET,
                                get_wrap_mode, NULL, NULL, NULL);

  g_settings_bind (self->editor_settings, "show-map",
                   self, "show-map",
                   G_SETTINGS_BIND_GET);

  g_settings_bind (self->editor_settings, "auto-hide-map",
                   self, "auto-hide-map",
                   G_SETTINGS_BIND_GET);

  g_signal_connect_object (self->editor_settings,
                           "changed::keybindings",
                           G_CALLBACK (on_keybindings_changed),
                           self,
                           G_CONNECT_SWAPPED);

  on_keybindings_changed (self, "keybindings", self->editor_settings);

  self->insight_settings = g_settings_new ("org.gnome.builder.code-insight");

  g_settings_bind (self->insight_settings, "word-completion",
                   source_view, "enable-word-completion",
                   G_SETTINGS_BIND_GET);
}
