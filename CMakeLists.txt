cmake_minimum_required(VERSION 3.22)
project(maec VERSION 1.0.0 DESCRIPTION "Modular Audio Engine in C++" HOMEPAGE_URL "https://github.com/Owen-Cochell/cmae" LANGUAGES CXX)

# Define our target and set properties:

add_library(${PROJECT_NAME}
    src/base_module.cpp
    src/audio_module.cpp
    src/module_mixer.cpp
    src/meta_audio.cpp
    src/base_oscillator.cpp
    src/fund_oscillator.cpp
    src/io/alsa_module.cpp
    src/audio_buffer.cpp
    src/sink_module.cpp
    src/amp_module.cpp
    src/module_param.cpp
    src/source_module.cpp
    src/chrono.cpp
    src/envelope.cpp
    src/utils.cpp
    src/filter_module.cpp
    src/io/wav.cpp
    src/dsp/conv.cpp
    src/dsp/ft.cpp
    src/dsp/util.cpp
    src/dsp/window.cpp
    src/dsp/kernel.cpp
    src/dsp/iir.cpp
    src/dsp/buffer.cpp
)

target_include_directories(${PROJECT_NAME}
    PUBLIC
        $<INSTALL_INTERFACE:include/${PROJECT_NAME}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/${PROJECT_NAME}>

    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Include helper modules:

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# See if we can find FFTW:

find_package(FFTW)

if (FFTW_FOUND)

  #####

  # Not super happy with this, should openMPI be required?
  # It is on Arch Linux, might not be the case for other systems...

  find_package(MPI)

  include_directories(${MPI_CXX_INCLUDE_DIRS})
  target_link_libraries(maec ${MPI_CXX_LIBRARIES})

  #####

  # Specify that we have found FFTW:

  target_compile_definitions(maec PUBLIC FFTW=1)

  # Include FFTW for linking:

  include_directories(${FFTW_INCLUDE_DIRS} SYSTEM)
  target_link_libraries(maec ${FFTW_LIBRARIES})

endif(FFTW_FOUND)

# Check for some platform-dependent stuff:

find_package(ALSA)

if(ALSA_FOUND)

  # Specify that we have found ALSA:

  target_compile_definitions(maec PUBLIC ALSA_F=1)

  # Include ALSA for linking:

  include_directories(${ALSA_INCLUDE_DIRS})
  target_link_libraries(maec ${ALSA_LIBRARIES})

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

add_subdirectory(test)
