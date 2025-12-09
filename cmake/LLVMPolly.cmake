# Function to enable LLVM Polly:
# https://polly.llvm.org/
# Enables Polly only if Clang supports it and only for Release configs.

include(CheckCXXSourceCompiles)  # <-- Ensure the check macro is available

# Probe once: can the configured Clang compile with -mllvm -polly?
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    if (NOT DEFINED CXX_COMPILER_HAS_POLLY)
        # Default to FALSE before probing
        set(CXX_COMPILER_HAS_POLLY FALSE CACHE BOOL "Clang supports Polly")

        # check_cxx_source_compiles honors CMAKE_REQUIRED_FLAGS
        set(_old_required_flags "${CMAKE_REQUIRED_FLAGS}")
        set(CMAKE_REQUIRED_FLAGS "${CMAKE_REQUIRED_FLAGS} -fpass-plugin=LLVMPolly.so -fplugin=LLVMPolly.so -mllvm -polly")
        check_cxx_source_compiles("int main(){return 0;}" COMPILER_HAS_POLY)
        set(CMAKE_REQUIRED_FLAGS "${_old_required_flags}")

        # Store the result in cache for global visibility
        if (COMPILER_HAS_POLY)
            set(CXX_COMPILER_HAS_POLLY TRUE CACHE BOOL "Clang supports Polly" FORCE)
        else()
            set(CXX_COMPILER_HAS_POLLY FALSE CACHE BOOL "Clang supports Polly" FORCE)
        endif()

        message(STATUS "Polly support probe: ${CXX_COMPILER_HAS_POLLY}")
    endif()
endif()

function(enable_polly target_name)
    if ((CMAKE_CXX_COMPILER_ID MATCHES "Clang") AND MAEC_POLY AND CXX_COMPILER_HAS_POLLY)
        if (CMAKE_CONFIGURATION_TYPES)
            target_compile_options(${target_name} PRIVATE
                # Enable polly functionality
                $<$<CONFIG:Release>:-fpass-plugin=LLVMPolly.so>
                $<$<CONFIG:Release>:-fplugin=LLVMPolly.so>
                # Polly pairs as single shell items:
                $<$<CONFIG:Release>:SHELL:-mllvm -polly>
                $<$<CONFIG:Release>:SHELL:-mllvm -polly-enable-optree>
                $<$<CONFIG:Release>:SHELL:-mllvm -polly-vectorizer=stripmine>
                $<$<CONFIG:Release>:SHELL:-mllvm -polly-invariant-load-hoisting>
                $<$<CONFIG:Release>:SHELL:-mllvm -polly-run-dce>
            )
        else()
            if (CMAKE_BUILD_TYPE STREQUAL "Release")
                target_compile_options(${target_name} PRIVATE
                    # Enable polly functionality
                    -fpass-plugin=LLVMPolly.so 
                    -fplugin=LLVMPolly.so
                    # Single-config: same fix
                    "SHELL:-mllvm -polly"
                    "SHELL:-mllvm -polly-enable-optree"
                    "SHELL:-mllvm -polly-vectorizer=stripmine"
                    "SHELL:-mllvm -polly-invariant-load-hoisting"
                    "SHELL:-mllvm -polly-run-dce"
                )
            endif()
        endif()
    endif()
endfunction()
