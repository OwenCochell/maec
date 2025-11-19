cmake_minimum_required(VERSION 3.25)

project(parallel-plot)

# Add the executable for this test:

add_executable( ${PROJECT_NAME} parallel_plot.cpp )

# Define C++ standard:
target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_20)

# Link maec and gnuplot

target_link_libraries( ${PROJECT_NAME} maec sciplot )
