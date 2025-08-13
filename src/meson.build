sources = ['main.cc',
                    'log.cc',
                    meson.project_source_root() + '/src/utils/cmdline.cc',
                    meson.project_source_root() + '/src/blocks/corr.cc',
                    meson.project_source_root() + '/src/dmr/sync.cc',
                    meson.project_source_root() + '/src/blocks/fconv.cc',
                    meson.project_source_root() + '/src/blocks/corr.cc',
                    meson.project_source_root() + '/src/blocks/firfilt.cc',
                    meson.project_source_root() + '/src/blocks/firinterp.cc' ]


executable('dmr-mon', sources,
    dependencies: [zmq_deps, volk_deps, fftw_deps ],
    include_directories : [ inc ])
