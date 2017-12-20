make_executable()

target_link_libraries(${PROJECT}
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
