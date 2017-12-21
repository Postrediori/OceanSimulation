file(GLOB_RECURSE PLOG_HEADERS ${CMAKE_SOURCE_DIR}/3rdparty/plog/*.h)

add_library(plog STATIC ${PLOG_HEADERS})

set_target_properties(plog PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(plog PROPERTIES FOLDER Include)

set(PLOG_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/3rdparty/plog/include)
set(PLOG_LIBRARY plog)
