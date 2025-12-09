# Applies all 3rd party optimizations to a target based on configuration

function(optimize target_name)

    # Determine if we should enable native optimizations

    if (MAEC_NATIVE)
        include(NativeBuild)
        enable_native(${target_name})
    endif()

    # Determine if we should enable Polly optimizations

    if (MAEC_POLY)
        include(LLVMPolly)
        enable_polly(${target_name})
    endif()
endfunction()
