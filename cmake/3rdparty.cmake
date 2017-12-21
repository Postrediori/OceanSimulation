set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/3rdparty")
find_package(OpenGL REQUIRED)
find_package(Freetype REQUIRED)

include(cmake/plog.cmake)
include(cmake/glad.cmake)
include(cmake/glfw.cmake)
include(cmake/glm.cmake)
