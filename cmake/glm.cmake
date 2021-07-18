set(GLM_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/3rdparty/glm)

add_library(glm INTERFACE)
target_include_directories(glm INTERFACE ${GLM_INCLUDE_DIR})

set(GLM_LIBRARY glm)
