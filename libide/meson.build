libide_header_dir = pkgincludedir + '/libide'

version_data = configuration_data()
version_data.set('MAJOR_VERSION', MAJOR_VERSION)
version_data.set('MINOR_VERSION', MINOR_VERSION)
version_data.set('MICRO_VERSION', MICRO_VERSION)
version_data.set('VERSION', meson.project_version())
version_data.set_quoted('BUILD_CHANNEL', get_option('with_channel'))
version_data.set_quoted('BUILD_TYPE', get_option('buildtype'))

libide_version_h = configure_file(
          input: 'ide-version.h.in',
         output: 'ide-version.h',
    install_dir: libide_header_dir,
        install: true,
  configuration: version_data)

libide_build_ident_h = vcs_tag(
     fallback: meson.project_version(),
        input: 'ide-build-ident.h.in',
       output: 'ide-build-ident.h',
      install: true,
  install_dir: libide_header_dir,
)

libide_enum_headers = [
  'buffers/ide-buffer.h',
  'buildsystem/ide-build-log.h',
  'buildsystem/ide-build-pipeline.h',
  'devices/ide-device.h',
  'diagnostics/ide-diagnostic.h',
  'doap/ide-doap.h',
  'files/ide-indent-style.h',
  'highlighting/ide-highlighter.h',
  'runtimes/ide-runtime.h',
  'sourceview/ide-cursor.h',
  'sourceview/ide-source-view.h',
  'symbols/ide-symbol.h',
  'threading/ide-thread-pool.h',
  'vcs/ide-vcs-config.h',
]

libide_enums = gnome.mkenums('ide-enums',
  h_template: 'ide-enums.h.in',
  c_template: 'ide-enums.c.in',
  sources: libide_enum_headers,
  install_header: true,
  install_dir: libide_header_dir,
)

libide_public_headers = [
  'ide-context.h',
  'ide-global.h',
  'ide-macros.h',
  'ide-object.h',
  'ide-service.h',
  'ide-types.h',
  'ide.h',
  'application/ide-application-addin.h',
  'application/ide-application-credits.h',
  'application/ide-application-tool.h',
  'application/ide-application.h',
  'buffers/ide-buffer-addin.h',
  'buffers/ide-buffer-change-monitor.h',
  'buffers/ide-buffer-manager.h',
  'buffers/ide-buffer.h',
  'buffers/ide-unsaved-file.h',
  'buffers/ide-unsaved-files.h',
  'buildconfig/ide-buildconfig-configuration.h',
  'buildconfig/ide-buildconfig-configuration-provider.h',
  'buildsystem/ide-build-log.h',
  'buildsystem/ide-build-manager.h',
  'buildsystem/ide-build-pipeline.h',
  'buildsystem/ide-build-pipeline-addin.h',
  'buildsystem/ide-build-stage.h',
  'buildsystem/ide-build-stage-launcher.h',
  'buildsystem/ide-build-stage-mkdirs.h',
  'buildsystem/ide-build-stage-transfer.h',
  'buildsystem/ide-build-system.h',
  'buildsystem/ide-build-system-discovery.h',
  'buildsystem/ide-build-target.h',
  'buildsystem/ide-build-utils.h',
  'buildsystem/ide-configuration-manager.h',
  'buildsystem/ide-configuration.h',
  'buildsystem/ide-configuration-provider.h',
  'buildsystem/ide-environment-variable.h',
  'buildsystem/ide-environment.h',
  'devices/ide-device-manager.h',
  'devices/ide-device-provider.h',
  'devices/ide-device.h',
  'diagnostics/ide-diagnostic-provider.h',
  'diagnostics/ide-diagnostic.h',
  'diagnostics/ide-diagnostics-manager.h',
  'diagnostics/ide-diagnostics.h',
  'diagnostics/ide-fixit.h',
  'diagnostics/ide-source-location.h',
  'diagnostics/ide-source-range.h',
  'directory/ide-directory-build-system.h',
  'directory/ide-directory-vcs.h',
  'doap/ide-doap-person.h',
  'doap/ide-doap.h',
  'documentation/ide-documentation.h',
  'documentation/ide-documentation-info.h',
  'documentation/ide-documentation-proposal.h',
  'documentation/ide-documentation-provider.h',
  'editor/ide-editor-addin.h',
  'editor/ide-editor-perspective.h',
  'editor/ide-editor-sidebar.h',
  'editor/ide-editor-utilities.h',
  'editor/ide-editor-view-addin.h',
  'editor/ide-editor-view.h',
  'files/ide-file-settings.h',
  'files/ide-file.h',
  'files/ide-indent-style.h',
  'formatting/ide-formatter.h',
  'formatting/ide-formatter-options.h',
  'genesis/ide-genesis-addin.h',
  'highlighting/ide-highlight-engine.h',
  'highlighting/ide-highlight-index.h',
  'highlighting/ide-highlighter.h',
  'history/ide-back-forward-controls.h',
  'history/ide-back-forward-item.h',
  'history/ide-back-forward-list.h',
  'langserv/ide-langserv-client.h',
  'langserv/ide-langserv-completion-provider.h',
  'langserv/ide-langserv-diagnostic-provider.h',
  'langserv/ide-langserv-formatter.h',
  'langserv/ide-langserv-highlighter.h',
  'langserv/ide-langserv-rename-provider.h',
  'langserv/ide-langserv-symbol-node.h',
  'langserv/ide-langserv-symbol-resolver.h',
  'langserv/ide-langserv-symbol-tree.h',
  'langserv/ide-langserv-util.h',
  'layout/ide-layout-grid.h',
  'layout/ide-layout-grid-column.h',
  'layout/ide-layout-stack.h',
  'layout/ide-layout-stack-addin.h',
  'layout/ide-layout-stack-header.h',
  'layout/ide-layout-view.h',
  'layout/ide-layout-pane.h',
  'layout/ide-layout-transient-sidebar.h',
  'layout/ide-layout.h',
  'local/ide-local-device.h',
  'logging/ide-log.h',
  'plugins/ide-extension-adapter.h',
  'plugins/ide-extension-set-adapter.h',
  'preferences/ide-preferences-addin.h',
  'preferences/ide-preferences-perspective.h',
  'projects/ide-project-edit.h',
  'projects/ide-project-file.h',
  'projects/ide-project-files.h',
  'projects/ide-project-info.h',
  'projects/ide-project-item.h',
  'projects/ide-project-miner.h',
  'projects/ide-project.h',
  'projects/ide-recent-projects.h',
  'rename/ide-rename-provider.h',
  'runner/ide-run-button.h',
  'runner/ide-run-manager.h',
  'runner/ide-runner.h',
  'runner/ide-runner-addin.h',
  'runtimes/ide-runtime-manager.h',
  'runtimes/ide-runtime-provider.h',
  'runtimes/ide-runtime.h',
  'search/ide-search-engine.h',
  'search/ide-search-entry.h',
  'search/ide-search-provider.h',
  'search/ide-search-result.h',
  'snippets/ide-source-snippet-chunk.h',
  'snippets/ide-source-snippet-context.h',
  'snippets/ide-source-snippet.h',
  'snippets/ide-source-snippets-manager.h',
  'snippets/ide-source-snippets.h',
  'sourceview/ide-completion-item.h',
  'sourceview/ide-completion-provider.h',
  'sourceview/ide-completion-results.h',
  'sourceview/ide-completion-words.h',
  'sourceview/ide-cursor.h',
  'sourceview/ide-indenter.h',
  'sourceview/ide-language.h',
  'sourceview/ide-source-map.h',
  'sourceview/ide-source-style-scheme.h',
  'sourceview/ide-source-view-mode.h',
  'sourceview/ide-source-view.h',
  'subprocess/ide-subprocess.h',
  'subprocess/ide-subprocess-launcher.h',
  'subprocess/ide-subprocess-supervisor.h',
  'symbols/ide-symbol-node.h',
  'symbols/ide-symbol-resolver.h',
  'symbols/ide-symbol-tree.h',
  'symbols/ide-symbol.h',
  'symbols/ide-tags-builder.h',
  'template/ide-project-template.h',
  'template/ide-template-base.h',
  'template/ide-template-provider.h',
  'threading/ide-thread-pool.h',
  'transfers/ide-pkcon-transfer.h',
  'transfers/ide-transfer-manager.h',
  'transfers/ide-transfer-row.h',
  'transfers/ide-transfer.h',
  'transfers/ide-transfer-button.h',
  'transfers/ide-transfers-button.h',
  'transfers/ide-transfers-progress-icon.h',
  'util/ide-cell-renderer-fancy.h',
  'util/ide-fancy-tree-view.h',
  'util/ide-flatpak.h',
  'util/ide-glib.h',
  'util/ide-gtk.h',
  'util/ide-line-reader.h',
  'util/ide-list-inline.h',
  'util/ide-posix.h',
  'util/ide-progress.h',
  'util/ide-settings.h',
  'util/ide-uri.h',
  'vcs/ide-vcs-config.h',
  'vcs/ide-vcs-initializer.h',
  'vcs/ide-vcs-uri.h',
  'vcs/ide-vcs.h',
  'workbench/ide-omni-bar.h',
  'workbench/ide-perspective.h',
  'workbench/ide-workbench-addin.h',
  'workbench/ide-workbench-message.h',
  'workbench/ide-workbench-header-bar.h',
  'workbench/ide-workbench.h',
  'workers/ide-worker.h',
]

libide_conf = configuration_data()
libide_conf.set10('ENABLE_TRACING', get_option('enable_tracing'))
libide_conf.set('BUGREPORT_URL', 'https://bugzilla.gnome.org/enter_bug.cgi?product=gnome-builder')
libide_debug_h = configure_file(
  input: 'ide-debug.h.in',
  output: 'ide-debug.h',
  configuration: libide_conf,
  install: true,
  install_dir: libide_header_dir,
)

# TODO: This is ugly and should be handled upstream
foreach header: libide_public_headers
  header_split = header.split('/')
  if header_split.length() == 2
    header_path = join_paths(libide_header_dir, header_split[0])
    install_headers(header, install_dir: header_path)
  else
    install_headers(header, install_dir: libide_header_dir)
  endif
endforeach

libide_resources = gnome.compile_resources('ide-resources',
  'libide.gresource.xml',
  c_name: 'ide',
)

libide_icons_resources = gnome.compile_resources('ide-icons-resources',
  join_paths(meson.source_root(), 'data/icons/hicolor/icons.gresource.xml'),
  source_dir: join_paths(meson.source_root(), 'data/icons/hicolor'),
  c_name: 'ide_icons',
)

libide_public_sources = [
  'application/ide-application-addin.c',
  'application/ide-application-tool.c',
  'application/ide-application.c',
  'application/ide-application-open.c',
  'buffers/ide-buffer-addin.c',
  'buffers/ide-buffer-change-monitor.c',
  'buffers/ide-buffer-manager.c',
  'buffers/ide-buffer.c',
  'buffers/ide-unsaved-file.c',
  'buffers/ide-unsaved-files.c',
  'buildconfig/ide-buildconfig-configuration.c',
  'buildconfig/ide-buildconfig-configuration-provider.c',
  'buildsystem/ide-build-manager.c',
  'buildsystem/ide-build-pipeline.c',
  'buildsystem/ide-build-pipeline-addin.c',
  'buildsystem/ide-build-stage.c',
  'buildsystem/ide-build-stage-launcher.c',
  'buildsystem/ide-build-stage-mkdirs.c',
  'buildsystem/ide-build-stage-transfer.c',
  'buildsystem/ide-build-system.c',
  'buildsystem/ide-build-system-discovery.c',
  'buildsystem/ide-build-target.c',
  'buildsystem/ide-build-utils.c',
  'buildsystem/ide-configuration-manager.c',
  'buildsystem/ide-configuration.c',
  'buildsystem/ide-configuration-provider.c',
  'buildsystem/ide-environment-variable.c',
  'buildsystem/ide-environment.c',
  'devices/ide-device-manager.c',
  'devices/ide-device-provider.c',
  'devices/ide-device.c',
  'diagnostics/ide-diagnostic-provider.c',
  'diagnostics/ide-diagnostic.c',
  'diagnostics/ide-diagnostics-manager.c',
  'diagnostics/ide-diagnostics.c',
  'diagnostics/ide-fixit.c',
  'diagnostics/ide-source-location.c',
  'diagnostics/ide-source-range.c',
  'directory/ide-directory-build-system.c',
  'directory/ide-directory-plugin.c',
  'directory/ide-directory-vcs.c',
  'doap/ide-doap-person.c',
  'doap/ide-doap.c',
  'documentation/ide-documentation.c',
  'documentation/ide-documentation-info.c',
  'documentation/ide-documentation-proposal.c',
  'documentation/ide-documentation-provider.c',
  'editor/ide-editor-addin.c',
  'editor/ide-editor-perspective.c',
  'editor/ide-editor-sidebar.c',
  'editor/ide-editor-utilities.c',
  'editor/ide-editor-view-addin.c',
  'editor/ide-editor-view.c',
  'files/ide-file-settings.c',
  'files/ide-file.c',
  'formatting/ide-formatter.c',
  'formatting/ide-formatter-options.c',
  'genesis/ide-genesis-addin.c',
  'highlighting/ide-highlight-engine.c',
  'highlighting/ide-highlight-index.c',
  'highlighting/ide-highlighter.c',
  'history/ide-back-forward-controls.c',
  'history/ide-back-forward-item.c',
  'history/ide-back-forward-list-load.c',
  'history/ide-back-forward-list-save.c',
  'history/ide-back-forward-list.c',
  'ide-context.c',
  'ide-object.c',
  'ide-service.c',
  'ide.c',
  'langserv/ide-langserv-client.c',
  'langserv/ide-langserv-completion-provider.c',
  'langserv/ide-langserv-diagnostic-provider.c',
  'langserv/ide-langserv-formatter.c',
  'langserv/ide-langserv-highlighter.c',
  'langserv/ide-langserv-rename-provider.c',
  'langserv/ide-langserv-symbol-node.c',
  'langserv/ide-langserv-symbol-node-private.h',
  'langserv/ide-langserv-symbol-resolver.c',
  'langserv/ide-langserv-symbol-tree.c',
  'langserv/ide-langserv-symbol-tree-private.h',
  'langserv/ide-langserv-util.c',
  'layout/ide-layout-grid.c',
  'layout/ide-layout-grid-column.c',
  'layout/ide-layout-pane.c',
  'layout/ide-layout-stack.c',
  'layout/ide-layout-stack-addin.c',
  'layout/ide-layout-stack-header.c',
  'layout/ide-layout-transient-sidebar.c',
  'layout/ide-layout-view.c',
  'layout/ide-layout.c',
  'local/ide-local-device.c',
  'logging/ide-log.c',
  'plugins/ide-extension-adapter.c',
  'plugins/ide-extension-set-adapter.c',
  'preferences/ide-preferences-addin.c',
  'preferences/ide-preferences-perspective.c',
  'projects/ide-project-edit.c',
  'projects/ide-project-file.c',
  'projects/ide-project-files.c',
  'projects/ide-project-info.c',
  'projects/ide-project-item.c',
  'projects/ide-project-miner.c',
  'projects/ide-project.c',
  'projects/ide-recent-projects.c',
  'rename/ide-rename-provider.c',
  'runner/ide-run-button.c',
  'runner/ide-run-manager.c',
  'runner/ide-runner.c',
  'runner/ide-runner-addin.c',
  'runtimes/ide-runtime-manager.c',
  'runtimes/ide-runtime-provider.c',
  'runtimes/ide-runtime.c',
  'search/ide-search-engine.c',
  'search/ide-search-entry.c',
  'search/ide-search-provider.c',
  'search/ide-search-result.c',
  'snippets/ide-source-snippet-chunk.c',
  'snippets/ide-source-snippet-context.c',
  'snippets/ide-source-snippet.c',
  'snippets/ide-source-snippets-manager.c',
  'snippets/ide-source-snippets.c',
  'sourceview/ide-completion-item.c',
  'sourceview/ide-completion-provider.c',
  'sourceview/ide-completion-results.c',
  'sourceview/ide-completion-words.c',
  'sourceview/ide-cursor.c',
  'sourceview/ide-indenter.c',
  'sourceview/ide-language.c',
  'sourceview/ide-source-map.c',
  'sourceview/ide-source-style-scheme.c',
  'sourceview/ide-source-view-mode.c',
  'sourceview/ide-source-view.c',
  'subprocess/ide-subprocess.c',
  'subprocess/ide-subprocess-launcher.c',
  'subprocess/ide-subprocess-supervisor.c',
  'symbols/ide-symbol-node.c',
  'symbols/ide-symbol-resolver.c',
  'symbols/ide-symbol-tree.c',
  'symbols/ide-symbol.c',
  'symbols/ide-tags-builder.c',
  'template/ide-project-template.c',
  'template/ide-template-base.c',
  'template/ide-template-provider.c',
  'threading/ide-thread-pool.c',
  'transfers/ide-pkcon-transfer.c',
  'transfers/ide-transfer-manager.c',
  'transfers/ide-transfer-row.c',
  'transfers/ide-transfer.c',
  'transfers/ide-transfer-button.c',
  'transfers/ide-transfers-button.c',
  'transfers/ide-transfers-progress-icon.c',
  'util/ide-cell-renderer-fancy.c',
  'util/ide-fancy-tree-view.c',
  'util/ide-flatpak.c',
  'util/ide-glib.c',
  'util/ide-gtk.c',
  'util/ide-line-reader.c',
  'util/ide-posix.c',
  'util/ide-progress.c',
  'util/ide-settings.c',
  'util/ide-uri.c',
  'vcs/ide-vcs-config.c',
  'vcs/ide-vcs-initializer.c',
  'vcs/ide-vcs-uri.c',
  'vcs/ide-vcs.c',
  'workbench/ide-omni-bar.c',
  'workbench/ide-perspective.c',
  'workbench/ide-workbench-addin.c',
  'workbench/ide-workbench-message.c',
  'workbench/ide-workbench-header-bar.c',
  'workbench/ide-workbench-open.c',
  'workbench/ide-workbench.c',
  'workers/ide-worker.c',
  libide_enums[0],
]

libide_generated_headers = [
  libide_build_ident_h,
  libide_resources[1],
  libide_icons_resources[1],
  libide_enums[1],
  libide_debug_h,
  libide_version_h,
]

libide_args = []

libide_sources = libide_generated_headers + libide_public_sources + [
  'application/ide-application-actions.c',
  'application/ide-application-actions.h',
  'application/ide-application-command-line.c',
  'application/ide-application-plugins.c',
  'application/ide-application-private.h',
  'application/ide-application-shortcuts.c',
  'application/ide-application-tests.c',
  'application/ide-application-tests.h',
  'buffers/ide-buffer-private.h',
  'buildconfig/ide-buildconfig-plugin.c',
  'buildconfig/ide-buildconfig-pipeline-addin.c',
  'buildconfig/ide-buildconfig-pipeline-addin.h',
  'buildsystem/ide-build-log.c',
  'buildsystem/ide-build-log-private.h',
  'buildsystem/ide-build-stage-private.h',
  'buildui/ide-build-configuration-row.c',
  'buildui/ide-build-configuration-row.h',
  'buildui/ide-build-configuration-view.c',
  'buildui/ide-build-configuration-view.h',
  'buildui/ide-build-log-panel.c',
  'buildui/ide-build-log-panel.h',
  'buildui/ide-build-panel.c',
  'buildui/ide-build-panel.h',
  'buildui/ide-build-perspective.c',
  'buildui/ide-build-perspective.h',
  'buildui/ide-build-plugin.c',
  'buildui/ide-build-tool.c',
  'buildui/ide-build-tool.h',
  'buildui/ide-build-workbench-addin.c',
  'buildui/ide-build-workbench-addin.h',
  'buildui/ide-environment-editor-row.c',
  'buildui/ide-environment-editor-row.h',
  'buildui/ide-environment-editor.c',
  'buildui/ide-environment-editor.h',
  'editor/ide-editor-layout-stack-addin.c',
  'editor/ide-editor-layout-stack-addin.h',
  'editor/ide-editor-layout-stack-controls.c',
  'editor/ide-editor-layout-stack-controls.h',
  'editor/ide-editor-perspective-actions.c',
  'editor/ide-editor-perspective-shortcuts.c',
  'editor/ide-editor-plugin.c',
  'editor/ide-editor-print-operation.c',
  'editor/ide-editor-print-operation.h',
  'editor/ide-editor-private.h',
  'editor/ide-editor-properties.c',
  'editor/ide-editor-properties.h',
  'editor/ide-editor-search-bar.c',
  'editor/ide-editor-search-bar-actions.c',
  'editor/ide-editor-search-bar-shortcuts.c',
  'editor/ide-editor-search-bar.h',
  'editor/ide-editor-view-actions.c',
  'editor/ide-editor-view-settings.c',
  'editor/ide-editor-view-shortcuts.c',
  'editor/ide-editor-workbench-addin.c',
  'editor/ide-editor-workbench-addin.h',
  'gconstructor.h',
  'greeter/ide-greeter-perspective.c',
  'greeter/ide-greeter-perspective.h',
  'greeter/ide-greeter-project-row.c',
  'greeter/ide-greeter-project-row.h',
  'gsettings/ide-gsettings-file-settings.c',
  'gsettings/ide-gsettings-file-settings.h',
  'gsettings/ide-language-defaults.c',
  'gsettings/ide-language-defaults.h',
  'history/ide-back-forward-list-private.h',
  'ide-internal.h',
  'keybindings/ide-keybindings.c',
  'keybindings/ide-keybindings.h',
  'keybindings/ide-shortcuts-window.c',
  'keybindings/ide-shortcuts-window.h',
  'layout/ide-layout-grid-actions.c',
  'layout/ide-layout-grid-column-actions.c',
  'layout/ide-layout-private.h',
  'layout/ide-layout-stack-actions.c',
  'layout/ide-layout-stack-shortcuts.c',
  'layout/ide-shortcut-label.c', # todo: libdazzle
  'layout/ide-shortcut-label.h', # todo: libdazzle
  'modelines/ide-modelines-file-settings.c',
  'modelines/ide-modelines-file-settings.h',
  'modelines/modeline-parser.c',
  'modelines/modeline-parser.h',
  'plugins/ide-extension-util.c',
  'plugins/ide-extension-util.h',
  'projects/ide-project-edit-private.h',
  'preferences/ide-preferences-builtin.c',
  'preferences/ide-preferences-builtin.h',
  'preferences/ide-preferences-language-row.c',
  'preferences/ide-preferences-language-row.h',
  'runner/ide-run-manager-private.h',
  'search/ide-search-reducer.c',
  'search/ide-search-reducer.h',
  'snippets/ide-source-snippet-completion-item.c',
  'snippets/ide-source-snippet-completion-item.h',
  'snippets/ide-source-snippet-completion-provider.c',
  'snippets/ide-source-snippet-completion-provider.h',
  'snippets/ide-source-snippet-parser.c',
  'snippets/ide-source-snippet-parser.h',
  'snippets/ide-source-snippet-private.h',
  'sourceview/ide-line-change-gutter-renderer.c',
  'sourceview/ide-line-change-gutter-renderer.h',
  'sourceview/ide-line-diagnostics-gutter-renderer.c',
  'sourceview/ide-line-diagnostics-gutter-renderer.h',
  'sourceview/ide-source-iter.c',
  'sourceview/ide-source-iter.h',
  'sourceview/ide-source-view-capture.c',
  'sourceview/ide-source-view-capture.h',
  'sourceview/ide-source-view-movements.c',
  'sourceview/ide-source-view-movements.h',
  'sourceview/ide-source-view-private.h',
  'sourceview/ide-source-view-shortcuts.c',
  'sourceview/ide-text-iter.c',
  'sourceview/ide-text-iter.h',
  'sourceview/ide-text-util.c',
  'sourceview/ide-text-util.h',
  'subprocess/ide-breakout-subprocess.c',
  'subprocess/ide-breakout-subprocess.h',
  'subprocess/ide-breakout-subprocess-private.h',
  'subprocess/ide-simple-subprocess.c',
  'subprocess/ide-simple-subprocess.h',
  'util/ide-async-helper.c',
  'util/ide-async-helper.h',
  'util/ide-battery-monitor.c',
  'util/ide-battery-monitor.h',
  'util/ide-doc-seq.c',
  'util/ide-doc-seq.h',
  'util/ide-ref-ptr.c',
  'util/ide-ref-ptr.h',
  'util/ide-window-settings.c',
  'util/ide-window-settings.h',
  'workbench/ide-omni-bar-row.c',
  'workbench/ide-omni-bar-row.h',
  'workbench/ide-workbench-actions.c',
  'workbench/ide-workbench-private.h',
  'workbench/ide-workbench-shortcuts.c',
  'workers/ide-worker-manager.c',
  'workers/ide-worker-manager.h',
  'workers/ide-worker-process.c',
  'workers/ide-worker-process.h',
]

libgtksource_dep = dependency('gtksourceview-3.0', version: '>= 3.22.0')

libide_deps = [
  dependency('pangoft2', version: '>= 1.38.0'),
  libgtksource_dep,
  libgio_dep,
  libgiounix_dep,
  libgtk_dep,
  libpeas_dep,
  libdazzle_dep,
  libtemplate_glib_dep,
  libxml_dep,
  libgd_dep,
  libjson_glib_dep,
  libjsonrpc_glib_dep,
]

contrib_dir = join_paths(meson.source_root(), 'contrib/')

if get_option('with_webkit')
  libide_sources += ['webkit/ide-webkit.c']
  libide_deps += [dependency('webkit2gtk-4.0', version: '>=2.12.0')]
endif

if get_option('with_editorconfig')
  libide_args += '-DENABLE_EDITORCONFIG'
  libide_deps += libeditorconfig_dep
  libide_sources += [
    'editorconfig/editorconfig-glib.c',
    'editorconfig/ide-editorconfig-file-settings.c',
  ]
endif

libide_api_version = '1.0'

# We want to find the subdirectory to install our override into:
python3 = find_program('python3')

get_overridedir = '''
import os
import sysconfig

libdir = sysconfig.get_config_var('LIBDIR')
if not libdir:
  libdir = '/usr/lib'

try:
  import gi
  overridedir = gi._overridesdir
except ImportError:
  purelibdir = sysconfig.get_path('purelib')
  overridedir = os.path.join(purelibdir, 'gi', 'overrides')

if overridedir.startswith(libdir): # Should always be True..
  overridedir = overridedir[len(libdir) + 1:]

print(overridedir)
'''

ret = run_command([python3, '-c', get_overridedir])
if ret.returncode() != 0
  error('Failed to determine pygobject overridedir')
else
  pygobject_override_dir = join_paths(get_option('libdir'), ret.stdout().strip())
endif

install_data('Ide.py', install_dir: pygobject_override_dir)

libide = shared_library('ide-' + libide_api_version,
  libide_resources + libide_icons_resources + libide_sources,
  dependencies: libide_deps,
  link_depends: 'ide.map',
        c_args: libide_args,
     link_args: [ '-Wl,--version-script,' + join_paths(meson.current_source_dir(), 'ide.map') ],
       install: true,
   install_dir: pkglibdir,
 install_rpath: pkglibdir_abs,
)

libide_dep = declare_dependency(
              sources: libide_generated_headers,
         dependencies: [ libdazzle_dep,
                         libgd_dep,
                         libgio_dep,
                         libgtk_dep,
                         libgtksource_dep,
                         libjson_glib_dep,
                         libtemplate_glib_dep,
                         libjsonrpc_glib_dep ],
            link_with: libide,
  include_directories: include_directories('.'),
)

# Doesn't link to libide
# TODO: I think we can remove most of the links here and just setup includes
libide_plugin_dep = declare_dependency(
              sources: libide_generated_headers,
  include_directories: include_directories('.'),
         dependencies: [ libdazzle_dep,
                         libgd_dep,
                         libgio_dep,
                         libgtk_dep,
                         libgtksource_dep,
                         libtemplate_glib_dep,
                         libjson_glib_dep,
                         libjsonrpc_glib_dep ],
)

pkgg = import('pkgconfig')

pkgg.generate(
    libraries: [libide],
      subdirs: [ 'gnome-builder-@0@/libide'.format(meson.project_version()) ],
      version: meson.project_version(),
         name: 'Libide',
     filebase: 'libide-1.0',
  description: 'Libide contains the components used to build the GNOME Builder IDE.',
     requires: [ 'gtk+-3.0', 'gtksourceview-3.0', 'libdazzle-1.0', 'template-glib-1.0', 'jsonrpc-glib-1.0' ],
  install_dir: join_paths(pkglibdir, 'pkgconfig'),
)

if get_option('with_introspection')

  libide_gir = gnome.generate_gir(libide,
                sources: libide_generated_headers + libide_public_headers + libide_public_sources,
              nsversion: libide_api_version,
              namespace: 'Ide',
           dependencies: [ libgd_dep ],
          symbol_prefix: 'ide',
      identifier_prefix: 'Ide',
               includes: [ 'Gio-2.0', 'GtkSource-3.0', 'Peas-1.0', libdazzle_gir, 'Json-1.0', libtemplate_glib_gir ],
                install: true,
        install_dir_gir: pkggirdir,
    install_dir_typelib: pkgtypelibdir,
             extra_args: [ '--c-include=ide.h' ]
  )

  if get_option('with_vapi')

    configure_file(
              input: 'libide-' + libide_api_version + '.deps',
             output: 'libide-' + libide_api_version + '.deps',
      configuration: configuration_data(),
            install: true,
        install_dir: pkgvapidir,
    )

    libide_vapi = gnome.generate_vapi('libide-' + libide_api_version,
          sources: libide_gir[0],
          install: true,
      install_dir: pkgvapidir,
         packages: [
           'gio-2.0',
           'gtk+-3.0',
           'gtksourceview-3.0',
           'json-glib-1.0',
           libdazzle_vapi,
           'libpeas-1.0',
           libtemplate_glib_vapi,
         ],
    )

  endif
endif
