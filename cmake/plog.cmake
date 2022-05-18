set(PLOG_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/3rdparty/plog/include)

add_library(plog INTERFACE)
target_include_directories(plog INTERFACE ${PLOG_INCLUDE_DIR})

set(PLOG_LIBRARY plog)
