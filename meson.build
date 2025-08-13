project('dmr-mon', 'cpp',
	version : '0.0.1',
	license : 'GPL3',
	meson_version : '>= 0.60.0')

zmq_deps = dependency('libzmq', version: '>= 4.3.0')
volk_deps = dependency('volk', version:'>= 2.5')
fftw_deps = dependency('fftw3f', version: '>= 3.0.0')

inc = include_directories(
	[ 'src', 'test', 'src/utils', 'src/dmr', 'src/blocks' ])

subdir('src')
subdir('test')
