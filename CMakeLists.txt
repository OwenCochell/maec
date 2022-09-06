cmake_minimum_required(VERSION 3.22)
project(cmae VERSION 1.0.0 DESCRIPTION "A Modular Audio Engine" HOMEPAGE_URL "https://github.com/Owen-Cochell/cmae" LANGUAGES CXX)

# Place the binary in the root of the project:

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR})

# Define our target and set properties:

add_library(cmae
    src/base_module.cpp
)

target_include_directories(cmae
    PUBLIC
        $<INSTALL_INTERFACE:include/cmae>    
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/cmae>

    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Set compile options:

target_compile_options(ojae PRIVATE -Wall -Wextra)
target_compile_features(ojae PRIVATE cxx_std_17)

# Add binaries in 'test':

add_subdirectory(tests)