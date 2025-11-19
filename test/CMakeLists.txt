project(Test)

# Unit tests managed by Catch2

set(TEST_FILES
    parameter_test.cpp
    module_mixers_test.cpp
    sink_module_test.cpp
    utils_test.cpp
    meta_module_test.cpp
    fund_oscillator_test.cpp
    envelope_test.cpp
    chrono_test.cpp
    audio_mod_test.cpp
    amp_module_test.cpp
    audio_buffer_test.cpp
    parallel_test.cpp
    io/mstream_test.cpp
    io/wav_test.cpp
    io/alsa_module_test.cpp
    dsp/buffer_test.cpp
    dsp/window_test.cpp
    dsp/kernel_test.cpp
    dsp/conv_test.cpp
    dsp/ft_test.cpp
)

# Enable testing for the project
enable_testing()

# Adding the tests target
add_executable(test ${TEST_FILES})

# Make Catch2 available to this file
FetchContent_MakeAvailable(Catch2)

# Linking test with the Catch2 libraries
target_link_libraries(test PRIVATE maec Catch2::Catch2WithMain)

# Add Catch2 extra modules to the CMAKE_MODULE_PATH
list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)

# Include Catch2 Test for testing
include(Catch)

catch_discover_tests(test)
