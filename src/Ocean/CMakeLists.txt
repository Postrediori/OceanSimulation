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
    ${CMAKE_SOURCE_DIR}/data/ocean.cfg
    ${CMAKE_CURRENT_BINARY_DIR}/data/ocean.cfg COPYONLY)

if (USE_OPENGL2_0)
    configure_file(
        ${CMAKE_SOURCE_DIR}/data/ocean110.vert
        ${CMAKE_CURRENT_BINARY_DIR}/data/ocean.vert COPYONLY)
    configure_file(
        ${CMAKE_SOURCE_DIR}/data/ocean110.frag
        ${CMAKE_CURRENT_BINARY_DIR}/data/ocean.frag COPYONLY)
else ()
    configure_file(
        ${CMAKE_SOURCE_DIR}/data/ocean.vert
        ${CMAKE_CURRENT_BINARY_DIR}/data/ocean.vert COPYONLY)
    configure_file(
        ${CMAKE_SOURCE_DIR}/data/ocean.frag
        ${CMAKE_CURRENT_BINARY_DIR}/data/ocean.frag COPYONLY)
endif ()

# Screen post-processing shaders
if (USE_OPENGL2_0)
    configure_file(
        ${CMAKE_SOURCE_DIR}/data/screen110.vert
        ${CMAKE_CURRENT_BINARY_DIR}/data/screen.vert COPYONLY)
    configure_file(
        ${CMAKE_SOURCE_DIR}/data/screen110-normal.frag
        ${CMAKE_CURRENT_BINARY_DIR}/data/screen-normal.frag COPYONLY)
    configure_file(
        ${CMAKE_SOURCE_DIR}/data/screen110-gray.frag
        ${CMAKE_CURRENT_BINARY_DIR}/data/screen-gray.frag COPYONLY)
    configure_file(
        ${CMAKE_SOURCE_DIR}/data/screen110-blur.frag
        ${CMAKE_CURRENT_BINARY_DIR}/data/screen-blur.frag COPYONLY)
    configure_file(
        ${CMAKE_SOURCE_DIR}/data/screen110-sobel.frag
        ${CMAKE_CURRENT_BINARY_DIR}/data/screen-sobel.frag COPYONLY)
    configure_file(
        ${CMAKE_SOURCE_DIR}/data/screen110-dither.frag
        ${CMAKE_CURRENT_BINARY_DIR}/data/screen-dither.frag COPYONLY)
    configure_file(
        ${CMAKE_SOURCE_DIR}/data/screen110-dither-gb.frag
        ${CMAKE_CURRENT_BINARY_DIR}/data/screen-dither-gb.frag COPYONLY)
else ()
    configure_file(
        ${CMAKE_SOURCE_DIR}/data/screen.vert
        ${CMAKE_CURRENT_BINARY_DIR}/data/screen.vert COPYONLY)
    configure_file(
        ${CMAKE_SOURCE_DIR}/data/screen-normal.frag
        ${CMAKE_CURRENT_BINARY_DIR}/data/screen-normal.frag COPYONLY)
    configure_file(
        ${CMAKE_SOURCE_DIR}/data/screen-gray.frag
        ${CMAKE_CURRENT_BINARY_DIR}/data/screen-gray.frag COPYONLY)
    configure_file(
        ${CMAKE_SOURCE_DIR}/data/screen-blur.frag
        ${CMAKE_CURRENT_BINARY_DIR}/data/screen-blur.frag COPYONLY)
    configure_file(
        ${CMAKE_SOURCE_DIR}/data/screen-sobel.frag
        ${CMAKE_CURRENT_BINARY_DIR}/data/screen-sobel.frag COPYONLY)
    configure_file(
        ${CMAKE_SOURCE_DIR}/data/screen-dither.frag
        ${CMAKE_CURRENT_BINARY_DIR}/data/screen-dither.frag COPYONLY)
    configure_file(
        ${CMAKE_SOURCE_DIR}/data/screen-dither-gb.frag
        ${CMAKE_CURRENT_BINARY_DIR}/data/screen-dither-gb.frag COPYONLY)
endif ()
