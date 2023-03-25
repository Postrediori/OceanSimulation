find_package(OpenGL REQUIRED)

if (USE_OPENGL2_0)
    message(STATUS "The project will use legacy OpenGL 2.0 for rendering")
else ()
    message(STATUS "The project will use OpenGL 3.3 for rendering")
endif ()

if (CMAKE_SYSTEM_NAME STREQUAL Linux)
  find_package(X11 REQUIRED)

  if (NOT X11_Xi_FOUND)
    message(FATAL_ERROR "X11 Xi library is required")
  endif ()
endif ()

include(cmake/plog.cmake)
include(cmake/glad.cmake)
include(cmake/glfw.cmake)
include(cmake/glm.cmake)
include(cmake/imgui.cmake)
include(cmake/stb.cmake)
