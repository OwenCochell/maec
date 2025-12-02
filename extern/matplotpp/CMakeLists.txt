#=============================================================================
# Add matplot++, a C++ graphing wrapper

cmake_minimum_required(VERSION 3.25)

include(FetchContent)

FetchContent_Declare(matplotpp
  GIT_REPOSITORY https://github.com/alandefreitas/matplotplusplus
  GIT_TAG v1.2.2
  SYSTEM
)

FetchContent_MakeAvailable(matplotpp)
