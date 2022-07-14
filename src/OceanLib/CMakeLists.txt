make_library()

target_precompile_headers(${PROJECT} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/stdafx.h)

target_include_directories(${PROJECT} PUBLIC
    ${GLM_INCLUDE_DIR}
    ${OPENGL_INCLUDE_DIR}
    ${GLFW_INCLUDE_DIR}
    ${GLAD_INCLUDE_DIR}
    ${FREETYPE_INCLUDE_DIRS}
    )

target_link_libraries(${PROJECT}
    ${PLOG_LIBRARY}
    MathLib
    FftLib
    GraphicsLib
    )
