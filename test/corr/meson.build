corr_test = executable('test-corr', 
    'test-corr.cc',
    test_sources,
    dependencies: [zmq_deps],
    include_directories : [ inc ])

test('correlation', corr_test)
