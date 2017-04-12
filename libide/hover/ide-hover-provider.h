/* ide-hover-provider.h
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

#ifndef IDE_HOVER_PROVIDER_H
#define IDE_HOVER_PROVIDER_H

#include "ide-types.h"

G_BEGIN_DECLS

#define IDE_TYPE_HOVER_PROVIDER (ide_hover_provider_get_type())

G_DECLARE_INTERFACE (IdeHoverProvider, ide_hover_provider, IDE, HOVER_PROVIDER, IdeObject)

struct _IdeHoverProviderInterface
{
  GTypeInterface parent_iface;

  void   (*query_async)  (IdeHoverProvider      *self,
                          IdeBuffer             *buffer,
                          const GtkTextIter     *location,
                          GCancellable          *cancellable,
                          GAsyncReadyCallback    callback,
                          gpointer               user_data);

  gchar *(*query_finish) (IdeHoverProvider      *self,
                          GAsyncResult          *result,
                          GError              **error);
};

G_END_DECLS

#endif /* IDE_HOVER_PROVIDER_H */
