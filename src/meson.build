sources = ['main.cc',
                    'log.cc',
                    meson.project_source_root() + '/src/utils/cmdline.cc',
                    meson.project_source_root() + '/src/dsp/vconv.cc',
                    meson.project_source_root() + '/src/dmr/sync.cc' ]


executable('dmr-mon', sources,
    dependencies: [zmq_deps, volk_deps ],
    include_directories : [ inc ])
