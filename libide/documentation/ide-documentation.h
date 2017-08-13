/* ide-documentation.h
 *
 * Copyright (C) 2017 Lucie Charvat <luci.charavt@gmail.com>
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

#ifndef IDE_DOCUMENTATION_H
#define IDE_DOCUMENTATION_H

#include <gtksourceview/gtksource.h>

#include "ide-documentation-info.h"

G_BEGIN_DECLS

#define IDE_TYPE_DOCUMENTATION (ide_documentation_get_type())

G_DECLARE_FINAL_TYPE (IdeDocumentation,
                      ide_documentation,
                      IDE, DOCUMENTATION,
                      IdeObject)

IdeDocumentationInfo    *ide_documentation_get_info    (IdeDocumentation        *self,
                                                        const gchar             *input,
                                                        IdeDocumentationContext  context);

G_END_DECLS

#endif /* IDE_DOCUMENTATION_H */
