/* ide-editor-sidebar.h
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

#pragma once

#include "layout/ide-layout-pane.h"

G_BEGIN_DECLS

#define IDE_TYPE_EDITOR_SIDEBAR (ide_editor_sidebar_get_type())

G_DECLARE_FINAL_TYPE (IdeEditorSidebar, ide_editor_sidebar, IDE, EDITOR_SIDEBAR, IdeLayoutPane)

GtkWidget   *ide_editor_sidebar_new            (void);
const gchar *ide_editor_sidebar_get_section_id (IdeEditorSidebar *self);
void         ide_editor_sidebar_set_section_id (IdeEditorSidebar *self,
                                                const gchar      *section_id);
void         ide_editor_sidebar_add_section    (IdeEditorSidebar *self,
                                                const gchar      *id,
                                                const gchar      *title,
                                                const gchar      *icon_name,
                                                const gchar      *menu_id,
                                                const gchar      *menu_icon_name,
                                                GtkWidget        *section,
                                                gint              priority);

G_END_DECLS
