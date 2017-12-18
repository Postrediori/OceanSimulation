make_library()

include_directories(
    ${OPENGL_INCLUDE_DIR}
    ${FREEGLUT_INCLUDE_DIRS}
    ${GLEW_INCLUDE_DIRS}
    ${GLM_INCLUDE_DIRS}
    ${FREETYPE_INCLUDE_DIRS}
    )
    
target_link_libraries(${PROJECT}
    ${OPENGL_LIBRARIES}
    ${FREEGLUT_LIBRARIES}
    ${GLEW_LIBRARIES}
    ${FREETYPE_LIBRARIES}
    )
    
if(MSVC)
    add_definitions(
        -D_USE_MATH_DEFINES
        -DFREEGLUT_LIB_PRAGMAS=0
        -DFREEGLUT_STATIC
        -DGLEW_STATIC
        )
endif()
