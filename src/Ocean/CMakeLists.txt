make_executable()

target_precompile_headers(${PROJECT} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/stdafx.h)

target_include_directories(${PROJECT} PUBLIC
    ${PLOG_INCLUDE_DIR}
    ${GLM_INCLUDE_DIR}
    ${OPENGL_INCLUDE_DIR}
    ${GLFW_INCLUDE_DIR}
    ${GLAD_INCLUDE_DIR}
    ${IMGUI_INCLUDE_DIR}
    ${STB_INCLUDE_DIR}
    )

target_link_libraries(${PROJECT}
    ${OPENGL_LIBRARIES}
    ${GLFW_LIBRARIES}
    ${GLAD_LIBRARIES}
    ${IMGUI_LIBRARIES}
    MathLib
    FftLib
    GraphicsLib
    OceanLib
    ScreenCaptureLib
    )

# Data files
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/data/ocean.cfg
    ${CMAKE_CURRENT_BINARY_DIR}/data/ocean.cfg COPYONLY)
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/data/ocean.vert
    ${CMAKE_CURRENT_BINARY_DIR}/data/ocean.vert COPYONLY)
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/data/ocean.frag
    ${CMAKE_CURRENT_BINARY_DIR}/data/ocean.frag COPYONLY)

# Screen post-processing shaders
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/data/screen.vert
    ${CMAKE_CURRENT_BINARY_DIR}/data/screen.vert COPYONLY)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/data/screen.frag
    ${CMAKE_CURRENT_BINARY_DIR}/data/screen.frag COPYONLY)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/data/screen-wave.frag
    ${CMAKE_CURRENT_BINARY_DIR}/data/screen-wave.frag COPYONLY)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/data/screen-bw.frag
    ${CMAKE_CURRENT_BINARY_DIR}/data/screen-bw.frag COPYONLY)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/data/screen-gray.frag
    ${CMAKE_CURRENT_BINARY_DIR}/data/screen-gray.frag COPYONLY)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/data/screen-blur.frag
    ${CMAKE_CURRENT_BINARY_DIR}/data/screen-blur.frag COPYONLY)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/data/screen-sobel.frag
    ${CMAKE_CURRENT_BINARY_DIR}/data/screen-sobel.frag COPYONLY)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/data/screen-dither.frag
    ${CMAKE_CURRENT_BINARY_DIR}/data/screen-dither.frag COPYONLY)


