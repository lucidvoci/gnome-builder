/* ide-gtk.h
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

#ifndef IDE_GTK_H
#define IDE_GTK_H

#include <gtk/gtk.h>

#include "ide-context.h"

#include "workbench/ide-workbench.h"

G_BEGIN_DECLS

typedef void (*IdeWidgetContextHandler) (GtkWidget  *widget,
                                         IdeContext *context);

void          ide_widget_set_context_handler (gpointer                 widget,
                                              IdeWidgetContextHandler  handler);
IdeContext   *ide_widget_get_context         (GtkWidget               *widget);
IdeWorkbench *ide_widget_get_workbench       (GtkWidget               *widget);

G_END_DECLS

#endif /* IDE_GTK_H */
