/* ide-xml-service.c
 *
 * Copyright (C) 2017 Sébastien Lafargue <slafargue@gnome.org>
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

#define G_LOG_DOMAIN "ide-xml-service"

#include <glib/gi18n.h>
#include <gtksourceview/gtksource.h>
#include <math.h>

#include "ide-xml-analysis.h"
#include "ide-xml-schema-cache-entry.h"
#include "ide-xml-tree-builder.h"
#include "ide-xml-types.h"

#include "ide-xml-service.h"

gboolean _ide_buffer_get_loading (IdeBuffer *self);

#define DEFAULT_EVICTION_MSEC (60 * 1000)

struct _IdeXmlService
{
  IdeObject          parent_instance;

  EggTaskCache      *analyses;
  EggTaskCache      *schemas;
  IdeXmlTreeBuilder *tree_builder;
  GCancellable      *cancellable;
};

static void service_iface_init (IdeServiceInterface *iface);

G_DEFINE_DYNAMIC_TYPE_EXTENDED (IdeXmlService, ide_xml_service, IDE_TYPE_OBJECT, 0,
                                G_IMPLEMENT_INTERFACE (IDE_TYPE_SERVICE, service_iface_init))

static void
ide_xml_service_build_tree_cb2 (GObject      *object,
                                GAsyncResult *result,
                                gpointer      user_data)
{
  IdeXmlTreeBuilder *tree_builder = (IdeXmlTreeBuilder *)object;
  g_autoptr(GTask) task = user_data;
  g_autoptr(IdeXmlAnalysis) analysis = NULL;
  GError *error = NULL;

  g_assert (IDE_IS_XML_TREE_BUILDER (tree_builder));
  g_assert (G_IS_TASK (result));
  g_assert (G_IS_TASK (task));

  if (NULL == (analysis = ide_xml_tree_builder_build_tree_finish (tree_builder, result, &error)))
    g_task_return_error (task, error);
  else
    g_task_return_pointer (task, g_steal_pointer (&analysis), (GDestroyNotify)ide_xml_analysis_unref);
}

static void
ide_xml_service_build_tree_cb (EggTaskCache  *cache,
                               gconstpointer  key,
                               GTask         *task,
                               gpointer       user_data)
{
  IdeXmlService *self = user_data;
  g_autofree gchar *path = NULL;
  IdeFile *ifile = (IdeFile *)key;
  GFile *gfile;

  IDE_ENTRY;

  g_assert (EGG_IS_TASK_CACHE (cache));
  g_assert (IDE_IS_XML_SERVICE (self));
  g_assert (IDE_IS_FILE (ifile));
  g_assert (G_IS_TASK (task));

  if (NULL == (gfile = ide_file_get_file (ifile)) ||
      NULL == (path = g_file_get_path (gfile)))
    {
      g_task_return_new_error (task,
                               G_IO_ERROR,
                               G_IO_ERROR_NOT_SUPPORTED,
                               _("File must be saved locally to parse."));
      return;
    }

  ide_xml_tree_builder_build_tree_async (self->tree_builder,
                                         gfile,
                                         g_task_get_cancellable (task),
                                         ide_xml_service_build_tree_cb2,
                                         g_object_ref (task));

  IDE_EXIT;
}

static void
ide_xml_service_load_schema_cb2 (GObject      *object,
                                 GAsyncResult *result,
                                 gpointer      user_data)
{
  GFile *file = (GFile *)object;
  g_autoptr(GTask) task = user_data;
  IdeXmlSchemaCacheEntry *cache_entry;
  GError *error = NULL;
  gchar *content;
  gsize len;

  g_assert (G_IS_FILE (file));
  g_assert (G_IS_TASK (result));
  g_assert (G_IS_TASK (task));

  cache_entry = ide_xml_schema_cache_entry_new ();

  if (!g_file_load_contents_finish (file, result, &content, &len, NULL, &error))
    cache_entry->error_message = g_strdup (error->message);
  else
    cache_entry->content = g_bytes_new_take (content, len);

  g_task_return_pointer (task, cache_entry, (GDestroyNotify)ide_xml_schema_cache_entry_unref);
}

static void
ide_xml_service_load_schema_cb (EggTaskCache  *cache,
                                gconstpointer  key,
                                GTask         *task,
                                gpointer       user_data)
{
  IdeXmlService *self = user_data;
  GFile *file = (GFile *)key;

  IDE_ENTRY;

  g_assert (EGG_IS_TASK_CACHE (cache));
  g_assert (IDE_IS_XML_SERVICE (self));
  g_assert (G_IS_TASK (task));
  g_assert (G_IS_FILE (file));

  g_file_load_contents_async (file,
                              g_task_get_cancellable (task),
                              ide_xml_service_load_schema_cb2,
                              g_object_ref (task));

  IDE_EXIT;
}

static void
ide_xml_service_get_analysis_cb (GObject      *object,
                                 GAsyncResult *result,
                                 gpointer      user_data)
{
  EggTaskCache *cache = (EggTaskCache *)object;
  g_autoptr(GTask) task = user_data;
  g_autoptr(IdeXmlAnalysis) analysis = NULL;
  GError *error = NULL;

  g_assert (EGG_IS_TASK_CACHE (cache));
  g_assert (G_IS_TASK (result));
  g_assert (G_IS_TASK (task));

  if (NULL == (analysis = egg_task_cache_get_finish (cache, result, &error)))
    g_task_return_error (task, error);
  else
    g_task_return_pointer (task, g_steal_pointer (&analysis), (GDestroyNotify)ide_xml_analysis_unref);
}

typedef struct
{
  IdeXmlService *self;
  GTask         *task;
  GCancellable  *cancellable;
  IdeFile       *ifile;
  IdeBuffer     *buffer;
} TaskState;

static void
ide_xml_service__buffer_loaded_cb (IdeBuffer *buffer,
                                   TaskState *state)
{
  IdeXmlService *self = (IdeXmlService *)state->self;

  g_assert (IDE_IS_XML_SERVICE (self));
  g_assert (G_IS_TASK (state->task));
  g_assert (state->cancellable == NULL || G_IS_CANCELLABLE (state->cancellable));
  g_assert (IDE_IS_FILE (state->ifile));
  g_assert (IDE_IS_BUFFER (state->buffer));

  g_signal_handlers_disconnect_by_func (buffer, ide_xml_service__buffer_loaded_cb, state);

  egg_task_cache_get_async (self->analyses,
                            state->ifile,
                            TRUE,
                            state->cancellable,
                            ide_xml_service_get_analysis_cb,
                            g_steal_pointer (&state->task));

  g_object_unref (state->buffer);
  g_object_unref (state->ifile);
  g_slice_free (TaskState, state);
}

static void
ide_xml_service_get_analysis_async (IdeXmlService       *self,
                                    IdeFile             *ifile,
                                    IdeBuffer           *buffer,
                                    GCancellable        *cancellable,
                                    GAsyncReadyCallback  callback,
                                    gpointer             user_data)
{
  g_autoptr(GTask) task = NULL;
  IdeContext *context;
  IdeBufferManager *manager;
  GFile *gfile;

  g_assert (IDE_IS_XML_SERVICE (self));
  g_assert (IDE_IS_FILE (ifile));
  g_assert (IDE_IS_BUFFER (buffer));
  g_assert (cancellable == NULL || G_IS_CANCELLABLE (cancellable));

  task = g_task_new (self, cancellable, callback, user_data);
  context = ide_object_get_context (IDE_OBJECT (self));
  manager = ide_context_get_buffer_manager (context);
  gfile = ide_file_get_file (ifile);

  if (!ide_buffer_manager_has_file (manager, gfile))
    {
      TaskState *state;

      if (!_ide_buffer_get_loading (buffer))
        {
          g_task_return_new_error (task,
                                   G_IO_ERROR,
                                   G_IO_ERROR_NOT_SUPPORTED,
                                   _("Buffer loaded but not in the buffer manager."));
          return;
        }

      /* Wait for the buffer to be fully loaded */
      state = g_slice_new0 (TaskState);
      state->self = self;
      state->task = g_steal_pointer (&task);
      state->cancellable = cancellable;
      state->ifile = g_object_ref (ifile);
      state->buffer = g_object_ref (buffer);

      g_signal_connect (buffer,
                        "loaded",
                        G_CALLBACK (ide_xml_service__buffer_loaded_cb),
                        state);
    }
  else
    egg_task_cache_get_async (self->analyses,
                              ifile,
                              TRUE,
                              cancellable,
                              ide_xml_service_get_analysis_cb,
                              g_steal_pointer (&task));
}

IdeXmlAnalysis *
ide_xml_service_get_analysis_finish (IdeXmlService  *self,
                                     GAsyncResult   *result,
                                     GError        **error)
{
  GTask *task = (GTask *)result;

  g_return_val_if_fail (IDE_IS_XML_SERVICE (self), NULL);
  g_return_val_if_fail (G_IS_TASK (task), NULL);

  return g_task_propagate_pointer (task, error);
}

static void
ide_xml_service_get_root_node_cb (GObject      *object,
                                  GAsyncResult *result,
                                  gpointer      user_data)
{
  IdeXmlService *self = (IdeXmlService *)object;
  g_autoptr(GTask) task = user_data;
  g_autoptr(IdeXmlAnalysis) analysis = NULL;
  IdeXmlSymbolNode *root_node;
  GError *error = NULL;

  g_assert (IDE_IS_XML_SERVICE (self));
  g_assert (G_IS_TASK (result));
  g_assert (G_IS_TASK (task));

  if (NULL == (analysis = ide_xml_service_get_analysis_finish (self, result, &error)))
    g_task_return_error (task, error);
  else
    {
      root_node = g_object_ref (ide_xml_analysis_get_root_node (analysis));
      g_task_return_pointer (task, root_node, g_object_unref);
    }
}

/**
 * ide_xml_service_get_root_node_async:
 *
 * This function is used to asynchronously retrieve the root node for
 * a particular file.
 *
 * If the root node is up to date, then no parsing will occur and the
 * existing root node will be used.
 *
 * If the root node is out of date, then the source file(s) will be
 * parsed asynchronously.
 *
 * The xml service is meant to be used with buffers, that is,
 * by extension, loaded views.
 */
void
ide_xml_service_get_root_node_async (IdeXmlService       *self,
                                     IdeFile             *ifile,
                                     IdeBuffer           *buffer,
                                     GCancellable        *cancellable,
                                     GAsyncReadyCallback  callback,
                                     gpointer             user_data)
{
  g_autoptr(GTask) task = NULL;
  IdeXmlAnalysis *cached;

  g_return_if_fail (IDE_IS_XML_SERVICE (self));
  g_return_if_fail (IDE_IS_FILE (ifile));
  g_return_if_fail (IDE_IS_BUFFER (buffer));
  g_return_if_fail (!cancellable || G_IS_CANCELLABLE (cancellable));

  task = g_task_new (self, cancellable, callback, user_data);

  /*
   * If we have a cached analysis with a valid root_node,
   * and it is new enough, then re-use it.
   */
  if (NULL != (cached = egg_task_cache_peek (self->analyses, ifile)))
    {
      IdeContext *context;
      IdeUnsavedFiles *unsaved_files;
      IdeUnsavedFile *uf;
      IdeXmlSymbolNode *root_node;
      GFile *gfile;

      gfile = ide_file_get_file (ifile);
      context = ide_object_get_context (IDE_OBJECT (self));
      unsaved_files = ide_context_get_unsaved_files (context);

      if (NULL != (uf = ide_unsaved_files_get_unsaved_file (unsaved_files, gfile)) &&
          ide_xml_analysis_get_sequence (cached) == ide_unsaved_file_get_sequence (uf))
        {
          root_node = g_object_ref (ide_xml_analysis_get_root_node (cached));
          g_assert (IDE_IS_XML_SYMBOL_NODE (root_node));

          g_task_return_pointer (task, root_node, g_object_unref);
          return;
        }
    }

  ide_xml_service_get_analysis_async (self,
                                      ifile,
                                      buffer,
                                      cancellable,
                                      ide_xml_service_get_root_node_cb,
                                      g_steal_pointer (&task));
}

/**
 * ide_xml_service_get_root_node_finish:
 *
 * Completes an asychronous request to get a root node for a given file.
 * See ide_xml_service_get_root_node_async() for more information.
 *
 * Returns: (transfer full): An #IdeXmlSymbolNode or %NULL up on failure.
 */
IdeXmlSymbolNode *
ide_xml_service_get_root_node_finish (IdeXmlService  *self,
                                      GAsyncResult   *result,
                                      GError        **error)
{
  GTask *task = (GTask *)result;

  g_return_val_if_fail (IDE_IS_XML_SERVICE (self), NULL);
  g_return_val_if_fail (G_IS_TASK (result), NULL);
  g_return_val_if_fail (error != NULL, NULL);

  return g_task_propagate_pointer (task, error);
}

static void
ide_xml_service_get_diagnostics_cb (GObject      *object,
                                    GAsyncResult *result,
                                    gpointer      user_data)
{
  IdeXmlService  *self = (IdeXmlService *)object;
  g_autoptr(GTask) task = user_data;
  g_autoptr(IdeXmlAnalysis) analysis = NULL;
  IdeDiagnostics *diagnostics;
  GError *error = NULL;

  g_assert (IDE_IS_XML_SERVICE (self));
  g_assert (G_IS_TASK (result));
  g_assert (G_IS_TASK (task));

  if (NULL == (analysis = ide_xml_service_get_analysis_finish (self, result, &error)))
    g_task_return_error (task, error);
  else
    {
      diagnostics = ide_diagnostics_ref (ide_xml_analysis_get_diagnostics (analysis));
      g_task_return_pointer (task, diagnostics, (GDestroyNotify)ide_diagnostics_unref);
    }
}

/**
 * ide_xml_service_get_diagnostics_async:
 *
 * This function is used to asynchronously retrieve the diagnostics
 * for a particular file.
 *
 * If the analysis is up to date, then no parsing will occur and the
 * existing diagnostics will be used.
 *
 * If the analysis is out of date, then the source file(s) will be
 * parsed asynchronously.
 *
 * The xml service is meant to be used with buffers, that is,
 * by extension, loaded views.
 */
void
ide_xml_service_get_diagnostics_async (IdeXmlService       *self,
                                       IdeFile             *ifile,
                                       IdeBuffer           *buffer,
                                       GCancellable        *cancellable,
                                       GAsyncReadyCallback  callback,
                                       gpointer             user_data)
{
  g_autoptr(GTask) task = NULL;
  IdeXmlAnalysis *cached;

  g_return_if_fail (IDE_IS_XML_SERVICE (self));
  g_return_if_fail (IDE_IS_FILE (ifile));
  g_return_if_fail (IDE_IS_BUFFER (buffer) || buffer == NULL);
  g_return_if_fail (!cancellable || G_IS_CANCELLABLE (cancellable));

  task = g_task_new (self, cancellable, callback, user_data);

  /*
   * If we have a cached analysis with some diagnostics,
   * and it is new enough, then re-use it.
   */
  if (NULL != (cached = egg_task_cache_peek (self->analyses, ifile)))
    {
      IdeContext *context;
      IdeUnsavedFiles *unsaved_files;
      IdeUnsavedFile *uf;
      IdeDiagnostics *diagnostics;
      GFile *gfile;

      gfile = ide_file_get_file (ifile);
      context = ide_object_get_context (IDE_OBJECT (self));
      unsaved_files = ide_context_get_unsaved_files (context);

      if (NULL != (uf = ide_unsaved_files_get_unsaved_file (unsaved_files, gfile)) &&
          ide_xml_analysis_get_sequence (cached) == ide_unsaved_file_get_sequence (uf))
        {
          diagnostics = ide_diagnostics_ref (ide_xml_analysis_get_diagnostics (cached));
          g_assert (diagnostics != NULL);

          g_task_return_pointer (task, diagnostics, (GDestroyNotify)ide_diagnostics_unref);
          return;
        }
    }

  ide_xml_service_get_analysis_async (self,
                                      ifile,
                                      buffer,
                                      cancellable,
                                      ide_xml_service_get_diagnostics_cb,
                                      g_steal_pointer (&task));
}

/**
 * ide_xml_service_get_diagnostics_finish:
 *
 * Completes an asychronous request to get the diagnostics for a given file.
 * See ide_xml_service_get_diagnostics_async() for more information.
 *
 * Returns: (transfer full): An #IdeDiagnostics or %NULL on failure.
 */
IdeDiagnostics *
ide_xml_service_get_diagnostics_finish (IdeXmlService  *self,
                                        GAsyncResult   *result,
                                        GError        **error)
{
  GTask *task = (GTask *)result;

  g_return_val_if_fail (IDE_IS_XML_SERVICE (self), NULL);
  g_return_val_if_fail (G_IS_TASK (result), NULL);
  g_return_val_if_fail (error != NULL, NULL);

  return g_task_propagate_pointer (task, error);
}

static void
ide_xml_service_context_loaded (IdeService *service)
{
  IdeXmlService *self = (IdeXmlService *)service;
  IdeContext *context;

  IDE_ENTRY;

  g_assert (IDE_IS_XML_SERVICE (self));

  context = ide_object_get_context (IDE_OBJECT (self));

  if (self->tree_builder == NULL)
    self->tree_builder = g_object_new (IDE_TYPE_XML_TREE_BUILDER,
                                       "context", context,
                                       NULL);

  IDE_EXIT;
}

typedef struct
{
  GTask         *task;
  IdeFile       *ifile;
  IdeBuffer     *buffer;
  gint           line;
  gint           line_offset;
} PositionState;

static void
position_state_free (PositionState *state)
{
  g_assert (state != NULL);

  g_object_unref (state->ifile);
  g_object_unref (state->buffer);
}

static IdeXmlPosition *
get_position (IdeXmlService   *self,
              IdeXmlAnalysis  *analysis,
              gint             line,
              gint             line_offset)
{
  IdeXmlPosition *position = NULL;
  IdeXmlSymbolNode *root_node;
  IdeXmlPositionKind kind;
  IdeXmlPositionKind candidate_kind = IDE_XML_POSITION_KIND_UNKNOW;
  IdeXmlSymbolNode *current_node;
  IdeXmlSymbolNode *parent_node = NULL;
  IdeXmlSymbolNode *child_node;
  IdeXmlSymbolNode *candidate_node = root_node;
  IdeXmlSymbolNode *previous_sibling_node = NULL;
  IdeXmlSymbolNode *next_sibling_node = NULL;
  guint n_children;
  gint n = 0;

  g_assert (IDE_IS_XML_SERVICE (self));
  g_assert (analysis != NULL);

  current_node = root_node = ide_xml_analysis_get_root_node (analysis);
  while (TRUE)
    {
loop:
      if (0 == (n_children = ide_xml_symbol_node_get_n_direct_children (current_node)))
        goto result;

      parent_node = current_node;
      for (n = 0; n < n_children; ++n)
        {
          child_node = IDE_XML_SYMBOL_NODE (ide_xml_symbol_node_get_nth_direct_child (current_node, n));
          kind = ide_xml_symbol_node_compare_location (child_node, line, line_offset);
          printf ("try kind: %s\n", ide_xml_position_kind_get_str (kind));
          switch (kind)
            {
            case IDE_XML_POSITION_KIND_IN_START_TAG:
            case IDE_XML_POSITION_KIND_IN_END_TAG:
              candidate_node = child_node;
              candidate_kind = kind;
              goto result;

            case IDE_XML_POSITION_KIND_BEFORE:
              /* We are in between two nodes, so let's take the parent node as candidate */
              goto result;

            case IDE_XML_POSITION_KIND_AFTER:
              /* We go to the next for loop iteration to test the next child */
              break;

            case IDE_XML_POSITION_KIND_IN_CONTENT:
              candidate_node = current_node = child_node;
              candidate_kind = kind;
              goto loop;

            case IDE_XML_POSITION_KIND_UNKNOW:
            default:
              g_assert_not_reached ();
            }
        }
    }

result:
  position = ide_xml_position_new (candidate_node, candidate_kind);
  ide_xml_position_set_analysis (position, analysis);

  if (parent_node != NULL)
    {
      n_children = ide_xml_symbol_node_get_n_direct_children (parent_node);
      if (n_children > 0)
        {
          if (n > 0)
            previous_sibling_node = IDE_XML_SYMBOL_NODE (ide_xml_symbol_node_get_nth_direct_child (parent_node, n - 1));

          if (n < n_children - 1)
            next_sibling_node = IDE_XML_SYMBOL_NODE (ide_xml_symbol_node_get_nth_direct_child (parent_node, n + 1));

          ide_xml_position_set_siblings (position, previous_sibling_node, next_sibling_node);
        }
    }

  return position;
}

static void
ide_xml_service_get_position_from_cursor_cb (GObject      *object,
                                             GAsyncResult *result,
                                             gpointer      user_data)
{
  IdeXmlService *self = (IdeXmlService *)object;
  PositionState *state = (PositionState *)user_data;
  g_autoptr(GTask) task = state->task;
  IdeXmlPosition *position;
  IdeXmlAnalysis *analysis = NULL;
  GError *error = NULL;

  IDE_ENTRY;

  g_assert (G_IS_TASK (task));
  g_assert (IDE_IS_XML_SERVICE (self));

  analysis = ide_xml_service_get_analysis_finish (self, result, &error);
  if (analysis != NULL)
    {
      position = get_position (self, analysis, state->line, state->line_offset);
      g_task_return_pointer (task, position, g_object_unref);
    }
  else
    g_task_return_error (task, error);

  position_state_free (state);

  IDE_EXIT;
}

void
ide_xml_service_get_position_from_cursor_async (IdeXmlService       *self,
                                                IdeFile             *ifile,
                                                IdeBuffer           *buffer,
                                                gint                 line,
                                                gint                 line_offset,
                                                GCancellable        *cancellable,
                                                GAsyncReadyCallback  callback,
                                                gpointer             user_data)
{
  PositionState *state;
  g_autoptr(GTask) task = NULL;

  IDE_ENTRY;

  g_return_if_fail (IDE_IS_XML_SERVICE (self));
  g_return_if_fail (IDE_IS_FILE (ifile));
  g_return_if_fail (IDE_IS_BUFFER (buffer) || buffer == NULL);
  g_return_if_fail (!cancellable || G_IS_CANCELLABLE (cancellable));

  task = g_task_new (self, cancellable, callback, user_data);

  state = g_slice_new0 (PositionState);
  state->task = g_steal_pointer (&task);
  state->ifile = g_object_ref (ifile);
  state->buffer = g_object_ref (buffer);
  state->line = line;
  state->line_offset = line_offset;

  ide_xml_service_get_analysis_async (self,
                                      ifile,
                                      buffer,
                                      cancellable,
                                      ide_xml_service_get_position_from_cursor_cb,
                                      state);

  IDE_EXIT;
}

IdeXmlPosition *
ide_xml_service_get_position_from_cursor_finish (IdeXmlService  *self,
                                                 GAsyncResult   *result,
                                                 GError        **error)
{
  GTask *task = (GTask *)result;

  g_return_val_if_fail (IDE_IS_XML_SERVICE (self), NULL);
  g_return_val_if_fail (G_IS_TASK (result), NULL);
  g_return_val_if_fail (error != NULL, NULL);

  return g_task_propagate_pointer (task, error);
}

static void
ide_xml_service_start (IdeService *service)
{
  IdeXmlService *self = (IdeXmlService *)service;

  g_assert (IDE_IS_XML_SERVICE (self));

  self->analyses = egg_task_cache_new ((GHashFunc)ide_file_hash,
                                       (GEqualFunc)ide_file_equal,
                                       g_object_ref,
                                       g_object_unref,
                                       (GBoxedCopyFunc)ide_xml_analysis_ref,
                                       (GBoxedFreeFunc)ide_xml_analysis_unref,
                                       DEFAULT_EVICTION_MSEC,
                                       ide_xml_service_build_tree_cb,
                                       self,
                                       NULL);

  egg_task_cache_set_name (self->analyses, "xml analysis cache");

  /* There's no eviction time on this cache */
  self->schemas = egg_task_cache_new ((GHashFunc)g_file_hash,
                                      (GEqualFunc)g_file_equal,
                                      g_object_ref,
                                      g_object_unref,
                                      (GBoxedCopyFunc)ide_xml_schema_cache_entry_ref,
                                      (GBoxedFreeFunc)ide_xml_schema_cache_entry_unref,
                                      0,
                                      ide_xml_service_load_schema_cb,
                                      self,
                                      NULL);

  egg_task_cache_set_name (self->schemas, "xml schemas cache");
}

static void
ide_xml_service_stop (IdeService *service)
{
  IdeXmlService *self = (IdeXmlService *)service;

  g_assert (IDE_IS_XML_SERVICE (self));

  if (self->cancellable && !g_cancellable_is_cancelled (self->cancellable))
    g_cancellable_cancel (self->cancellable);

  g_clear_object (&self->cancellable);
  g_clear_object (&self->analyses);
  g_clear_object (&self->schemas);
}

static void
ide_xml_service_finalize (GObject *object)
{
  IdeXmlService *self = (IdeXmlService *)object;

  IDE_ENTRY;

  ide_xml_service_stop (IDE_SERVICE (self));
  g_clear_object (&self->tree_builder);

  G_OBJECT_CLASS (ide_xml_service_parent_class)->finalize (object);

  IDE_EXIT;
}

static void
ide_xml_service_class_init (IdeXmlServiceClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = ide_xml_service_finalize;
}

static void
service_iface_init (IdeServiceInterface *iface)
{
  iface->context_loaded = ide_xml_service_context_loaded;
  iface->start = ide_xml_service_start;
  iface->stop = ide_xml_service_stop;
}

static void
ide_xml_service_class_finalize (IdeXmlServiceClass *klass)
{
}

static void
ide_xml_service_init (IdeXmlService *self)
{
}

void
_ide_xml_service_register_type (GTypeModule *module)
{
  ide_xml_service_register_type (module);
}

/**
 * ide_xml_service_get_cached_root_node:
 *
 * Gets the #IdeXmlSymbolNode root node for the corresponding file.
 *
 * Returns: (transfer NULL): A xml symbol node.
 */
IdeXmlSymbolNode *
ide_xml_service_get_cached_root_node (IdeXmlService *self,
                                      GFile         *gfile)
{
  IdeXmlAnalysis *analysis;
  IdeXmlSymbolNode *cached;

  g_return_val_if_fail (IDE_IS_XML_SERVICE (self), NULL);
  g_return_val_if_fail (IDE_IS_FILE (gfile), NULL);

  if (NULL != (analysis = egg_task_cache_peek (self->analyses, gfile)) &&
      NULL != (cached = ide_xml_analysis_get_root_node (analysis)))
    return g_object_ref (cached);

  return NULL;
}

/**
 * ide_xml_service_get_cached_diagnostics:
 *
 * Gets the #IdeDiagnostics for the corresponding file.
 *
 * Returns: (transfer NULL): A #IdeDiagnostics.
 */
IdeDiagnostics *
ide_xml_service_get_cached_diagnostics (IdeXmlService *self,
                                        GFile         *gfile)
{
  IdeXmlAnalysis *analysis;
  IdeDiagnostics *cached;

  g_return_val_if_fail (IDE_IS_XML_SERVICE (self), NULL);
  g_return_val_if_fail (IDE_IS_FILE (gfile), NULL);

  if (NULL != (analysis = egg_task_cache_peek (self->analyses, gfile)) &&
      NULL != (cached = ide_xml_analysis_get_diagnostics (analysis)))
    return ide_diagnostics_ref (cached);

  return NULL;
}

/**
 * ide_xml_service_get_schemas_cache:
 *
 * Gets the #EggTaskCache for the xml schemas.
 *
 * Returns: (transfer NULL): A #EggTaskCache.
 */
EggTaskCache *
ide_xml_service_get_schemas_cache (IdeXmlService *self)
{
  g_return_val_if_fail (IDE_IS_XML_SERVICE (self), NULL);

  return self->schemas;
}
