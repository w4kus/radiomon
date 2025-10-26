sources = ['main.cc',
                    'log.cc',
                    meson.project_source_root() + '/src/utils/cmdline.cc',
                    meson.project_source_root() + '/src/utils/corr.cc',
                    # meson.project_source_root() + '/src/dmr/sync.cc',
                    meson.project_source_root() + '/src/utils/fconv.cc',
                    meson.project_source_root() + '/src/utils/nco.cc',
                    meson.project_source_root() + '/src/utils/freq-est.cc']


executable('dmr-mon', sources,
    dependencies: [zmq_deps, volk_deps, fftw_deps ],
    include_directories : [ inc ])
