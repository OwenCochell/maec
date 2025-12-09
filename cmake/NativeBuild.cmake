# Tools to enable and query native CPU optimizations

# Determine if the compiler supports -march=native
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-march=native" COMPILER_SUPPORTS_MARCH_NATIVE)

# Function to enable native optimizations for a given target
function(enable_native target_name)
    if (COMPILER_SUPPORTS_MARCH_NATIVE AND MAEC_NATIVE)
        if (CMAKE_CONFIGURATION_TYPES)
            target_compile_options(${target_name} PRIVATE
                $<$<CONFIG:Release>:-march=native>
            )
        else()
            if (CMAKE_BUILD_TYPE STREQUAL "Release")
                target_compile_options(${target_name} PRIVATE
                    -march=native
                )
            endif()
        endif()
    endif()
endfunction()
