set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/3rdparty")
find_package(OpenGL REQUIRED)
find_package(FreeGLUT REQUIRED)
find_package(GLEW REQUIRED)
find_package(GLM REQUIRED)
find_package(Freetype REQUIRED)

include(cmake/plog.cmake)
