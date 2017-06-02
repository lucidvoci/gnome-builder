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
#define MAX_SEARCH 100

#include <fcntl.h>
#include <glib/gi18n.h>
#include <ide.h>
#include <webkit2/webkit2.h>

#include "gbp-devhelp-documentation-card.h"
#include "gbp-devhelp-workbench-addin.h"

struct _GbpDevhelpDocumentationCard
{
  GObject        parent_instance;
};


G_DEFINE_TYPE (GbpDevhelpDocumentationCard, gbp_devhelp_documentation_card, G_TYPE_OBJECT)

static void
gbp_devhelp_documentation_card_class_init (GbpDevhelpDocumentationCardClass *klass)
{

}

static void
gbp_devhelp_documentation_card_init (GbpDevhelpDocumentationCard *self)
{

}

void
gbp_devhelp_documentation_card_show_card (GbpDevhelpDocumentationCard *self)
{

}
