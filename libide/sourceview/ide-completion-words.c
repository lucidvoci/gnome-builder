/* ide-completion-words.c
 *
 * Copyright (C) 2016 Christian Hergert <chergert@redhat.com>
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

#define G_LOG_DOMAIN "ide-completion-words"

#include "ide-completion-provider.h"
#include "ide-completion-words.h"

#include "ide-context.h"
#include "documentation/ide-documentation.h"

struct _IdeCompletionWords
{
  GtkSourceCompletionWords parent_instance;

  IdeContext *context;
};

enum {
  PROP_0,
  PROP_CONTEXT,
  N_PROPS
};

static void completion_provider_init (GtkSourceCompletionProviderIface *iface);

G_DEFINE_TYPE_WITH_CODE (IdeCompletionWords, ide_completion_words, GTK_SOURCE_TYPE_COMPLETION_WORDS,
                         G_IMPLEMENT_INTERFACE (GTK_SOURCE_TYPE_COMPLETION_PROVIDER, completion_provider_init))

static GParamSpec *properties [N_PROPS];

static void
ide_completion_words_set_property (GObject      *object,
                                   guint         prop_id,
                                   const GValue *value,
                                   GParamSpec   *pspec)
{
  IdeCompletionWords *self = (IdeCompletionWords *)object;

  switch (prop_id)
    {
    case PROP_CONTEXT:
      ide_set_weak_pointer (&self->context, g_value_get_object (value));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
ide_completion_words_finalize (GObject *object)
{
  IdeCompletionWords *self = (IdeCompletionWords *)object;

  ide_clear_weak_pointer (&self->context);
}

static void
ide_completion_words_class_init (IdeCompletionWordsClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  object_class->set_property = ide_completion_words_set_property;
  object_class->finalize = ide_completion_words_finalize;

  properties [PROP_CONTEXT] =
    g_param_spec_object ("context",
                         "Context",
                         "The IdeContext for the documentation.",
                         IDE_TYPE_CONTEXT,
                         (G_PARAM_WRITABLE | G_PARAM_CONSTRUCT_ONLY | G_PARAM_STATIC_STRINGS));

  g_object_class_install_properties (object_class, N_PROPS, properties);
}

static void
ide_completion_words_init (IdeCompletionWords *self)
{
}

static gboolean
ide_completion_words_match (GtkSourceCompletionProvider *provider,
                            GtkSourceCompletionContext  *context)
{
  GtkSourceCompletionActivation activation;
  GtkTextIter iter;

  g_assert (IDE_IS_COMPLETION_WORDS (provider));
  g_assert (GTK_SOURCE_IS_COMPLETION_CONTEXT (context));

  activation = gtk_source_completion_context_get_activation (context);

  if (activation == GTK_SOURCE_COMPLETION_ACTIVATION_INTERACTIVE)
    {
      if (ide_completion_provider_context_in_comment (context))
        return FALSE;
    }

  if (!gtk_source_completion_context_get_iter (context, &iter))
    return FALSE;

  if (gtk_text_iter_backward_char (&iter))
    {
      gunichar ch = gtk_text_iter_get_char (&iter);

      if (!g_unichar_isalnum (ch) && ch != '_')
        return FALSE;
    }

  return TRUE;
}

static void
provider_update_info (GtkSourceCompletionProvider *provider,
                      GtkSourceCompletionProposal *proposal,
                      GtkSourceCompletionInfo     *info)
{
  g_autofree gchar *selected_text = NULL;
  selected_text = gtk_source_completion_proposal_get_text (proposal);
  if (selected_text == NULL)
    return;
}

static GtkWidget *
provider_get_info_widget (GtkSourceCompletionProvider *provider,
                          GtkSourceCompletionProposal *proposal)
{
  IdeCompletionWords *self = (IdeCompletionWords *)provider;

  IdeDocumentation *doc;
  g_autoptr(IdeDocumentationInfo) info = NULL;
  IdeDocumentationProposal *doc_proposal = NULL;
  g_autofree gchar *selected_text = NULL;
  g_autofree gchar *header_text = NULL;
  GtkWidget *label;

  doc = ide_context_get_documentation (self->context);
  selected_text = gtk_source_completion_proposal_get_text (proposal);
  if (selected_text == NULL)
    return NULL;

  info = ide_documentation_get_info (doc, selected_text, IDE_DOCUMENTATION_CONTEXT_CARD_C);
  if (ide_documentation_info_get_size (info) == 0)
    return NULL;

  doc_proposal = ide_documentation_info_get_proposal (info, 0);

  label = gtk_label_new (NULL);
  gtk_label_set_markup (GTK_LABEL (label), ide_documentation_proposal_get_header (doc_proposal));
  gtk_widget_show (label);

  return label;
}

static void
completion_provider_init (GtkSourceCompletionProviderIface *iface)
{
  iface->match = ide_completion_words_match;
  iface->update_info = provider_update_info;
  iface->get_info_widget = provider_get_info_widget;
}
