project(TTests)

# Unit tests managed by Catch2

set(TEST_FILES
    parameter_test.cpp
)

# Enable testing for the project
enable_testing()

# Get Google Tests
include(FetchContent)
FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG v3.4.0
)

FetchContent_MakeAvailable(Catch2)

# adding the Tests_run target
add_executable(tests ${TEST_FILES})

# linking Tests_run with the Google Test libraries
target_link_libraries(tests PRIVATE maec Catch2::Catch2WithMain)

# Add Catch2 extra modules to the CMAKE_MODULE_PATH:

list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)

# Include Google Test for testing
include(CTest)
include(Catch)

catch_discover_tests(tests)
