make_library()

target_precompile_headers(${PROJECT} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/stdafx.h)

target_link_libraries(${PROJECT}
    ${GLAD_LIBRARIES}
    ${GLFW_LIBRARIES}
    ${GLM_LIBRARY}
    ${IMGUI_LIBRARIES}
    ${OPENGL_LIBRARIES}
    ${PLOG_LIBRARY}
    )
