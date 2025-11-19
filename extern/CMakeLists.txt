cmake_minimum_required(VERSION 3.10.2)

# Group targets under one group

set(CMAKE_FOLDER extern)

# Include external targets:

add_subdirectory(sciplot)
add_subdirectory(catch2)
