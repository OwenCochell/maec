#=============================================================================
# Add sciplot, a C++ graphing wrapper

cmake_minimum_required(VERSION 3.25)

include(FetchContent)

# We don't want tests
option(SCIPLOT_BUILD_TESTS OFF)

# No docs either
option(SCIPLOT_BUILD_DOCS OFF)

# Finally, no examples
option(SCIPLOT_BUILD_EXAMPLES OFF)

FetchContent_Declare(sciplot_content
  GIT_REPOSITORY https://github.com/sciplot/sciplot.git
  GIT_TAG master
  SYSTEM
)

FetchContent_MakeAvailable(sciplot_content)
