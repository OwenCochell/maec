#=============================================================================
# Add matplot++, a C++ graphing wrapper

cmake_minimum_required(VERSION 3.25)

include(FetchContent)

# We don't want any examples
option(MATPLOTPP_BUILD_EXAMPLES OFF)

# We also don't want any tests
option(MATPLOTPP_BUILD_TESTS OFF)

# Disable master mode, is a dependency
option(MASTER_PROJECT OFF)

# No installer build
option(MATPLOTPP_BUILD_INSTALLER OFF)

# No package build
option(MATPLOTPP_BUILD_PACKAGE OFF)

FetchContent_Declare(matplotpp
  GIT_REPOSITORY https://github.com/alandefreitas/matplotplusplus
  GIT_TAG v1.2.2
  SYSTEM
)

FetchContent_MakeAvailable(matplotpp)
