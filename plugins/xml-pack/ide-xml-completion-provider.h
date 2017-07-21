/* ide-xml-completion-provider.h
 *
 * Copyright (C) 2017 Sebastien Lafargue <slafargue@gnome.org>
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

#ifndef IDE_XML_COMPLETION_PROVIDER_H
#define IDE_XML_COMPLETION_PROVIDER_H

#include <gtksourceview/gtksource.h>
#include <ide.h>

G_BEGIN_DECLS

#define IDE_TYPE_XML_COMPLETION_PROVIDER (ide_xml_completion_provider_get_type())

G_DECLARE_FINAL_TYPE (IdeXmlCompletionProvider, ide_xml_completion_provider, IDE, XML_COMPLETION_PROVIDER, IdeObject)

IdeXmlCompletionProvider *ide_xml_completion_provider_new (void);

G_END_DECLS

#endif /* IDE_XML_COMPLETION_PROVIDER_H */

