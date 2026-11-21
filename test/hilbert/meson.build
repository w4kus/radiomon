executable('test-hilbert',
    'test-hilbert.cc',
    test_sources,
    meson.project_source_root() + '/src/utils/zmq/context.cc',
    include_directories : [ inc ],
    dependencies: [ volk_deps, zmq_deps ])
