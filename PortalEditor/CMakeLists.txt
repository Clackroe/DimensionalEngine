cmake_minimum_required(VERSION 3.5)
project(PortalEditor)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)

# Source files
file(GLOB_RECURSE APP_SOURCES "src/*.cpp")

# Include directories
include_directories(${PROJECT_SOURCE_DIR}/../Dimensional-Core/src)
include_directories(${PROJECT_SOURCE_DIR}/../Dimensional-Core/lib/GLM)
include_directories(${PROJECT_SOURCE_DIR}/../Dimensional-Core/lib/ENTT/single_include)
include_directories(src)

# Executable
add_executable(Portal ${APP_SOURCES})
target_link_libraries(Portal Dimensional-Core)

add_custom_command(TARGET Portal POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${PROJECT_SOURCE_DIR}/Assets" ${CMAKE_BINARY_DIR}/Assets
    COMMENT "Copying ${PROJECT_SOURCE_DIR} to ${CMAKE_BINARY_DIR}/Assets"
)
