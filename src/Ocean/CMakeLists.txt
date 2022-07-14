make_executable()

target_precompile_headers(${PROJECT} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/stdafx.h)

target_link_libraries(${PROJECT}
    ${OPENGL_LIBRARIES}
    ${GLFW_LIBRARIES}
    ${GLAD_LIBRARIES}
    ${GLM_LIBRARY}
    ${IMGUI_LIBRARIES}
    ${PLOG_LIBRARY}
    ${STB_LIBRARY}
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

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/data/screen-dither-gb.frag
    ${CMAKE_CURRENT_BINARY_DIR}/data/screen-dither-gb.frag COPYONLY)

