cmake_minimum_required(VERSION 3.25)

project(iir-plot)

# Add the executable for this test:

add_executable( ${PROJECT_NAME} iir_plot.cpp )

# Define C++ standard:
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)

# Link maec and gnuplot

target_link_libraries( ${PROJECT_NAME} maec matplot )
