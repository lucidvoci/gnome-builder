/* gbp-documentation-card.c
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

#define G_LOG_DOMAIN "gbp-documentation-card"

#include <glib/gi18n.h>
#include <ide.h>

#include "gbp-documentation-card.h"

#define CARD_WIDTH             80

struct _GbpDocumentationCard
{
  GtkPopover    parent_instance;

  IdeEditorView  *view;
  GdkWindow      *window;
  GdkDevice      *pointer;

  GtkButton    *button;
  GtkEntry     *entry;
  GtkLabel     *header;
  GtkLabel     *text;

  gchar        *uri;
};

G_DEFINE_TYPE (GbpDocumentationCard, gbp_documentation_card, GTK_TYPE_POPOVER)

static void
gbp_documentation_card__button_clicked (GbpDocumentationCard *self,
                                        GtkButton            *button)
{
  g_assert (GBP_IS_DOCUMENTATION_CARD (self));
  g_assert (GTK_IS_BUTTON (button));

  gtk_widget_set_visible (GTK_WIDGET (self->text), TRUE);
  gtk_widget_set_visible (GTK_WIDGET (self->button), FALSE);

  gtk_popover_set_modal (GTK_POPOVER (self), TRUE);
  gtk_label_set_width_chars (self->text, CARD_WIDTH);

}

void
gbp_documentation_card__closed (gpointer    user_data,
                                GbpDocumentationCard  *self)
{
  gtk_popover_set_modal (GTK_POPOVER (self), FALSE);

  gtk_widget_set_visible (GTK_WIDGET (self->text), FALSE);
  gtk_widget_set_visible (GTK_WIDGET (self->button), TRUE);
}

static void
gbp_documentation_card_class_init (GbpDocumentationCardClass *klass)
{
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  gtk_widget_class_set_template_from_resource (widget_class, "/org/gnome/builder/plugins/documentation-card/gbp-documentation-card.ui");
  gtk_widget_class_bind_template_child (widget_class, GbpDocumentationCard, button);
  gtk_widget_class_bind_template_child (widget_class, GbpDocumentationCard, header);
  gtk_widget_class_bind_template_child (widget_class, GbpDocumentationCard, text);

}

static void
gbp_documentation_card_init (GbpDocumentationCard *self)
{

  gtk_widget_init_template (GTK_WIDGET (self));

  g_signal_connect_object (self->button,
                           "clicked",
                           G_CALLBACK (gbp_documentation_card__button_clicked),
                           self,
                           G_CONNECT_SWAPPED);

  g_signal_connect_object (self,
                           "closed",
                           G_CALLBACK (gbp_documentation_card__closed),
                           NULL,
                           G_CONNECT_SWAPPED);
}

void
gbp_documentation_card_set_info (GbpDocumentationCard *self,
                                 IdeDocumentationInfo *info)
{
  IdeDocumentationProposal *proposal;

  g_assert (GBP_IS_DOCUMENTATION_CARD (self));
  g_assert (IDE_IS_DOCUMENTATION_INFO (info));

  proposal = ide_documentation_info_get_proposal (info, 0);

  gtk_label_set_markup (self->text, ide_documentation_proposal_get_text (proposal));
  gtk_label_set_markup (self->header, ide_documentation_proposal_get_header (proposal));
}

void
gbp_documentation_card_popup (GbpDocumentationCard *self,
                              gint x,
                              gint y)
{
  GdkRectangle rec;

  g_return_if_fail (GBP_IS_DOCUMENTATION_CARD (self));

  if (x < 0 || y < 0)
    return;

  rec.x = x;
  rec.y = y;
  rec.width = 1;
  rec.height = 1;

  gtk_popover_set_pointing_to (GTK_POPOVER (self), &rec);
  gtk_popover_popup (GTK_POPOVER (self));
}

