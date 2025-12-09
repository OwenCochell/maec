# Applies all 3rd party optimizations to a target based on configuration

function(optimize target_name)

    # Determine if we should enable native optimizations

    if (MAEC_NATIVE)
        include(cmake/NativeBuild.cmake)
        enable_native(${target_name})
    endif()

    # Determine if we should enable Polly optimizations

    if (MAEC_POLY)
        include(cmake/LLVMPolly.cmake)
        enable_polly(${target_name})
    endif()
endfunction()
