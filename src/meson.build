sources = ['main.cc',
                    'log.cc',
                    meson.project_source_root() + '/utils/cmdline.cc',
                    meson.project_source_root() + '/src/dmr/sync.cc' ]


executable('dmr-mon', sources,
    dependencies: [zmq_deps ],
    include_directories : [ inc ])
