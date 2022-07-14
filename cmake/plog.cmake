set(PLOG_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/3rdparty/plog)

add_library(plog INTERFACE)
target_include_directories(plog INTERFACE ${PLOG_INCLUDE_DIR}/include)

set(PLOG_LIBRARY plog)
