set(GLAD_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/3rdparty/glad/include)
file(GLOB GLAD_SOURCES ${CMAKE_SOURCE_DIR}/3rdparty/glad/src/glad.c)

if(NOT WIN32)
    set(GLAD_LIBRARIES dl)
endif()
