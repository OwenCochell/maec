# Function to enable native CPU build for a target

option(MAEC_NATIVE_BUILD "Enables native CPU optimizations" ON)

include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-march=native" COMPILER_SUPPORTS_MARCH_NATIVE)

function(enable_native target_name)
    if (COMPILER_SUPPORTS_MARCH_NATIVE AND MAEC_NATIVE_BUILD)
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
