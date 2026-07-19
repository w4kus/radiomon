executable('test-zmq-publisher',
    'test-zmq-publisher.cc',
    test_sources,
    meson.project_source_root() + '/src/utils/zmq/context.cc',
    include_directories : [ inc ],
    dependencies: [ volk_deps, zmq_deps ]
)

executable('test-zmq-subscriber',
    'test-zmq-subscriber.cc',
    test_sources,
    meson.project_source_root() + '/src/utils/zmq/context.cc',
    include_directories : [ inc ],
    dependencies: [ volk_deps, zmq_deps ]
)
