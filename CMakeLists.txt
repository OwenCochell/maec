cmake_minimum_required(VERSION 3.22)
project(cmae VERSION 1.0.0 DESCRIPTION "A Modular Audio Engine" HOMEPAGE_URL "https://github.com/Owen-Cochell/cmae" LANGUAGES CXX)

# Define our target and set properties:

add_library(${PROJECT_NAME}
    src/base_module.cpp
    src/audio_module.cpp
    src/module_mixer.cpp
    src/meta_audio.cpp
    src/base_oscillator.cpp
    src/fund_oscillator.cpp
    src/alsa_module.cpp
    src/audio_buffer.cpp
    src/sink_module.cpp
    src/amp_module.cpp
    src/module_param.cpp
    src/source_module.cpp
    src/chrono.cpp
    src/envelope.cpp
    src/utils.cpp
)

target_include_directories(${PROJECT_NAME}
    PUBLIC
        $<INSTALL_INTERFACE:include/${PROJECT_NAME}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/${PROJECT_NAME}>

    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Check for some platform-dependent stuff:

find_package(ALSA)

if(ALSA_FOUND)

  # Specify that we have found ALSA:

  target_compile_definitions(cmae PUBLIC ALSA_F=1)

  # Include ALSA for linking:

  include_directories(${ALSA_INCLUDE_DIRS})
  target_link_libraries(cmae ${ALSA_LIBRARIES})
endif(ALSA_FOUND)

# Set compile options:

if(MSVC)
  target_compile_options(${PROJECT_NAME} PRIVATE /W4)
else()
  target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
endif()

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)

# Add binaries in 'demos'

add_subdirectory(demos)

# Add binaries in 'test':

add_subdirectory(tests)
