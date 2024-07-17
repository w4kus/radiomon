
sources = ['main.cc', 'corr.cc', 'log.cc']

executable('dmr-mon', sources,
    dependencies: [zmq_deps, fftw_deps],
    include_directories :
    get_option('boost'))
