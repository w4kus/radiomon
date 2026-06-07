project('radiomon', 'cpp',
	version : '0.0.1',
	license : 'MIT',
	default_options: ['cpp_std=c++14'],
	meson_version : '>= 0.60.0')

zmq_deps = dependency('libzmq', version: '>= 4.3.0')
volk_deps = dependency('volk', version:'>= 2.5')
fftw_deps = dependency('fftw3f', version: '>= 3.0.0')
portaudio_deps = dependency('portaudio-2.0', version: '>= 19')

inc = include_directories(
	[ 'src', 'test', 'src/utils', 'src/blocks', 'src/components',
	'src/blocks/endpoints', 'src/blocks/endpoints/audio-backends']
)

add_global_arguments('-fno-exceptions', language: 'cpp')

subdir('test')
