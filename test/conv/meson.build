executable('test-conv', 
    'test-conv.cc',
    meson.project_source_root() + '/src/dsp/convolution/vconv.cc',
    meson.project_source_root() + '/src/dsp/convolution/fconv.cc',
    test_sources,
    include_directories : [ inc ],
    dependencies: [volk_deps, fftw_deps])
