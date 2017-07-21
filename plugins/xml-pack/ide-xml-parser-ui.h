/* ide-xml-parser-ui.h
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

#ifndef IDE_XML_PARSER_UI_H
#define IDE_XML_PARSER_UI_H

#include <glib.h>

#include "ide-xml-parser-private.h"

G_BEGIN_DECLS

void      ide_xml_parser_ui_setup       (IdeXmlParser *self,
                                         ParserState  *state);

G_END_DECLS

#endif /* IDE_XML_PARSER_UI_H */
