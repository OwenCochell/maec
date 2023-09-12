project(Tests)

# Unit tests managed by GTest

set(TEST_FILES
    gtest_main.cpp
    audio_mod_test.cpp
    constant_oscillator_test.cpp
    module_mixers.cpp
    fund_oscillator_test.cpp
    #audio_buffer_test.cpp
    sink_module_test.cpp
    alsa_module_test.cpp
    meta_module_test.cpp
    amp_module_test.cpp
    parameter_test.cpp
    chrono_test.cpp
    envelope_test.cpp
    utils_test.cpp
    dsp/conv_test.cpp
    dsp/ft_test.cpp
    dsp/window_test.cpp
    dsp/kernel_test.cpp
    dsp/buffer_test.cpp
)

# Enable testing for the project
enable_testing()

# Get Google Tests
include(FetchContent)
FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG main
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Include Google Test directories
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

# adding the Tests_run target
add_executable(Tests_run ${TEST_FILES})

# linking Tests_run with the Google Test libraries
target_link_libraries(Tests_run PUBLIC maec gtest gtest_main)

# Include Google Test for testing
include(GoogleTest)
gtest_discover_tests(Tests_run)
