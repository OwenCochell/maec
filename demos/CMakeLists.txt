project(Demos)

# Demo files to be ran

set(MISC_FILES
    alsa_demo.cpp
    coversion_experiment.cpp
    container_benchmark.cpp
    fftw_demo.cpp
    fft_radix2_benchmark.cpp
    mstream_bench.cpp
)

# Finally, build and link all misc. demo files:

foreach( demosourcefile ${MISC_FILES} )

    # Determine a target name

    string( REPLACE ".cpp" "" demoname ${demosourcefile} )

    # Add the exacutable for this test:

    add_executable( ${demoname} ${demosourcefile} )

    # Link MAEC

    target_link_libraries( ${demoname} maec )

endforeach( demosourcefile ${MISC_FILES} )
