project('dmr-mon', 'cpp',
	version : '0.0.1',
	license : 'GPL3',
	meson_version : '>= 0.60.0')

zmq_deps = dependency('libzmq', version: '>= 4.3.0')

inc = include_directories([ 'src', 'test', 'utils', 'src/dmr' ])

subdir('src')
subdir('test')
