/* gb-project-tree-private.h
 *
 * Copyright (C) 2015 Christian Hergert <christian@hergert.me>
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

#ifndef GB_PROJECT_TREE_PRIVATE_H
#define GB_PROJECT_TREE_PRIVATE_H

#include <ide.h>

G_BEGIN_DECLS

struct _GbProjectTree
{
  DzlTree     parent_instance;

  GSettings *settings;

  guint      expanded_in_new : 1;
  guint      show_ignored_files : 1;
};

G_END_DECLS

#endif /* GB_PROJECT_TREE_PRIVATE_H */
