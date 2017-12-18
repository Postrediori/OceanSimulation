project(OceanSimulation)
cmake_minimum_required(VERSION 2.8)

# 3rd party libraries
include(cmake/3rdparty.cmake)

# macro and configurations
include(cmake/config.cmake)

# projects
add_subdirectory(Ocean)
