executable('test-audio-endpoint',
    'test-audio-endpoint.cc',
    test_sources,
    meson.project_source_root() + '/src/blocks/endpoints/audio-backends/backend-portaudio.cc',
    include_directories : [ inc ],
    dependencies: [ volk_deps, portaudio_deps ],
    cpp_args: [ '-DINCLUDE_AUDIO_ENDPOINTS', '-DAUDIO_BACKEND_PORTAUDIO' ]
)
