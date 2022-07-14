make_library()

target_precompile_headers(${PROJECT} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/stdafx.h)

target_include_directories(${PROJECT} PUBLIC
    ${OPENGL_INCLUDE_DIR}
    ${GLFW_INCLUDE_DIR}
    ${GLAD_INCLUDE_DIR}
    ${STB_INCLUDE_DIR}
    )

target_link_libraries(${PROJECT}
    ${OPENGL_LIBRARIES}
    ${GLFW_LIBRARIES}
    ${GLAD_LIBRARIES}
    ${PLOG_LIBRARY}
    )
