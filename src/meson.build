sources = ['main.cc',
                    'corr.cc',
                    'log.cc',
                    meson.project_source_root() + '/utils/cmdline.cc']

# FIXME - boost option needs to be removed if the dist has it already
executable('dmr-mon', sources,
    dependencies: [zmq_deps, boost_dep],
    include_directories :
        [ get_option('boostdir'), inc])
