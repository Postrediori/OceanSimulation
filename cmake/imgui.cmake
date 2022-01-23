set(IMGUI_DIR ${CMAKE_SOURCE_DIR}/3rdparty/imgui)
set(IMGUI_BACKENDS_DIR ${IMGUI_DIR}/backends)
file(GLOB IMGUI_SOURCES ${IMGUI_DIR}/*.cpp)
file(GLOB IMGUI_HEADERS ${IMGUI_DIR}/*.h)

set(IMGUI_INCLUDE_DIR ${IMGUI_DIR} ${INGUI_BACKENDS_DIR})
set(IMGUI_BACKENDS_SOURCES
    ${IMGUI_BACKENDS_DIR}/imgui_impl_opengl3.cpp
    ${IMGUI_BACKENDS_DIR}/imgui_impl_glfw.cpp)

add_library(imgui STATIC ${IMGUI_SOURCES} ${IMGUI_SOURCES} ${IMGUI_BACKENDS_SOURCES})

target_compile_definitions(imgui PUBLIC IMGUI_IMPL_OPENGL_LOADER_GLAD)

target_include_directories(imgui PUBLIC
    ${IMGUI_DIR} ${IMGUI_BACKENDS_DIR}
    ${OPENGL_INCLUDE_DIR}
    ${GLFW_INCLUDE_DIR}
    ${GLAD_INCLUDE_DIR}
    )

target_link_libraries(imgui
    ${OPENGL_LIBRARIES}
    ${GLFW_LIBRARIES}
    ${GLAD_LIBRARIES}
    )

set_target_properties(imgui PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(imgui PROPERTIES FOLDER 3rdparty)

set(IMGUI_LIBRARIES imgui)
