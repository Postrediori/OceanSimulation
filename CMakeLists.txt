cmake_minimum_required(VERSION 3.11)
project(OceanSimulation)

set(CMAKE_CXX_STANDARD 17)

option(USE_OPENGL2_0 "Use only OpenGL 2.0 for maximum hardware compatibility" OFF)

# 3rd party libraries
include(cmake/3rdparty.cmake)

# macro and configurations
include(cmake/config.cmake)

# projects
add_subdirectory(src)
