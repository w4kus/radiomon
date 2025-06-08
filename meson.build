project('dmr-mon', 'cpp',
	version : '0.0.1',
	license : 'GPL3',
	meson_version : '>= 0.60.0')

zmq_deps = dependency('libzmq', version: '>= 4.3.0')
volk_deps = dependency('volk', version:'>= 2.5')

inc = include_directories([ 'src', 'test', 'src/utils', 'src/dmr', 'src/dsp' ])

subdir('src')
subdir('test')
