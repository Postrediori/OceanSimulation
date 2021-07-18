project(OceanSimulation)
cmake_minimum_required(VERSION 3.11)

set(CMAKE_CXX_STANDARD 17)

# 3rd party libraries
include(cmake/3rdparty.cmake)

# macro and configurations
include(cmake/config.cmake)

# projects
add_subdirectory(src)
