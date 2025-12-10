project(Demos)

# Group targets under one group

set(CMAKE_FOLDER demos)

# Include demo subdirectories tha might have custom CMake requirements:

add_subdirectory( parallel_plot )

# Demo files to be ran

set(MISC_FILES
    alsa_demo.cpp
    coversion_experiment.cpp
    container_benchmark.cpp
    fftw_demo.cpp
    fft_radix2_benchmark.cpp
    mstream_bench.cpp
    char_conversion.cpp
    call_benchmark.cpp
    vector_pass.cpp
    chain_bench.cpp
    static_bench.cpp
    chain_bench_dyn.cpp
    parallel_bench.cpp
)

include(Optimize)

# Finally, build and link all misc. demo files:

foreach( demosourcefile ${MISC_FILES} )

    # Determine a target name

    string( REPLACE ".cpp" "" demoname ${demosourcefile} )

    # Add the exacutable for this test:

    add_executable( ${demoname} ${demosourcefile} )

    # Preform optimizations, if any

    optimize(${demoname})

    # Link MAEC

    target_link_libraries( ${demoname} maec )

endforeach( demosourcefile ${MISC_FILES} )
