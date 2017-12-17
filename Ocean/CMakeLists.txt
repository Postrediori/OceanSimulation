make_executable()

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
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/data/font.ttf
    ${CMAKE_CURRENT_BINARY_DIR}/data/font.ttf COPYONLY)
