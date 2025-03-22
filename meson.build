project('dmr-mon', 'cpp',
	version : '0.0.1',
	license : 'GPL3',
	meson_version : '>= 0.60.0')

zmq_deps = dependency('libzmq', version: '>= 4.3.0')
boost_dep = dependency('boost')

inc = include_directories([ 'src', 'test', 'utils' ])

subdir('src')
subdir('test')
