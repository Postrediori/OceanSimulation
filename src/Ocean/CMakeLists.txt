make_executable()

include_directories(
    ${PLOG_INCLUDE_DIR}
    ${GLM_INCLUDE_DIR}
    ${OPENGL_INCLUDE_DIR}
    ${GLFW_INCLUDE_DIR}
    ${GLAD_INCLUDE_DIR}
    ${FREETYPE_INCLUDE_DIRS}
    )
    
target_link_libraries(${PROJECT}
    ${OPENGL_LIBRARIES}
    ${GLFW_LIBRARIES}
    ${GLAD_LIBRARIES}
    ${FREETYPE_LIBRARIES}
    MathLib
    FftLib
    GraphicsLib
    OceanLib
    )

# Data files
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/data/ocean.cfg
    ${CMAKE_CURRENT_BINARY_DIR}/data/ocean.cfg COPYONLY)
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/data/ocean110.vert
    ${CMAKE_CURRENT_BINARY_DIR}/data/ocean110.vert COPYONLY)
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/data/ocean110.frag
    ${CMAKE_CURRENT_BINARY_DIR}/data/ocean110.frag COPYONLY)
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/data/ocean130.vert
    ${CMAKE_CURRENT_BINARY_DIR}/data/ocean130.vert COPYONLY)
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/data/ocean130.frag
    ${CMAKE_CURRENT_BINARY_DIR}/data/ocean130.frag COPYONLY)
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/data/font.ttf
    ${CMAKE_CURRENT_BINARY_DIR}/data/font.ttf COPYONLY)
