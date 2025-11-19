#=============================================================================
# Add Catch2 testing library, via FetchContent

cmake_minimum_required(VERSION 3.10.2)

include(FetchContent)

set(PACKAGE_NAME Catch2)

FetchContent_Declare(
  ${PACKAGE_NAME}
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG v3.8.1
  SYSTEM
)

FetchContent_MakeAvailable(${PACKAGE_NAME})
