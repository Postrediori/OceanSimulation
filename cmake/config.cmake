macro(make_project_)
    if (NOT DEFINED PROJECT)
        get_filename_component(PROJECT ${CMAKE_CURRENT_SOURCE_DIR} NAME)
    endif ()

    project(${PROJECT} CXX)

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")

    if(MSVC)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
    else ()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -std=c++11")
    endif ()

    if (NOT DEFINED HEADERS)
        file(GLOB HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
    endif ()

    if (NOT DEFINED SOURCES)
        file(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
    endif ()

    source_group("Header Files" FILES ${HEADERS})
    source_group("Source Files" FILES ${SOURCES})
endmacro ()

macro(make_executable)
    make_project_()
    
    add_executable(${PROJECT} ${HEADERS} ${SOURCES})
    
    include_directories(
        ${PLOG_INCLUDE_DIR}
        ${OPENGL_INCLUDE_DIR}
        ${FREEGLUT_INCLUDE_DIRS}
        ${GLEW_INCLUDE_DIRS}
        ${GLM_INCLUDE_DIRS}
        ${FREETYPE_INCLUDE_DIRS}
        )
        
    target_link_libraries(${PROJECT}
        ${PLOG_LIBRARY}
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
    
    set(CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}/bundle/${PROJECT}")
    install(
        TARGETS ${PROJECT}
        DESTINATION ${CMAKE_INSTALL_PREFIX})
    install(
        DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/data"
        DESTINATION ${CMAKE_INSTALL_PREFIX})
endmacro()

macro(make_library)
    make_project_()
    add_library(${PROJECT} STATIC ${HEADERS} ${SOURCES})
    target_include_directories(${PROJECT} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

    include_directories(
        ${PLOG_INCLUDE_DIR}
        )

    target_link_libraries(${PROJECT}
        ${PLOG_LIBRARY}
        )

    if (NOT SOURCES)
        set_target_properties(${PROJECT} PROPERTIES LINKER_LANGUAGE CXX)
    endif ()
endmacro()

function(add_all_subdirectories)
    file(GLOB CHILDREN RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/*)

    foreach(CHILD ${CHILDREN})
        if (IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${CHILD})
            add_subdirectory(${CHILD})
        endif ()
    endforeach ()
endfunction()
