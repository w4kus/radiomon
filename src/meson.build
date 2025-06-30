sources = ['main.cc',
                    'log.cc',
                    meson.project_source_root() + '/src/utils/cmdline.cc',
                    meson.project_source_root() + '/src/dsp/convolution/vconv.cc',
                    meson.project_source_root() + '/src/dmr/sync.cc',
                    meson.project_source_root() + '/src/dsp/convolution/fconv.cc' ]


executable('dmr-mon', sources,
    dependencies: [zmq_deps, volk_deps, fftw_deps ],
    include_directories : [ inc ])
