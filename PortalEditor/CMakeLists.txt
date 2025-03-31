cmake_minimum_required(VERSION 3.5)
project(PortalEditor)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)

# Source files
# file(GLOB_RECURSE APP_SOURCES "src/*.cpp")


# Include directories
include_directories(${PROJECT_SOURCE_DIR}/../Dimensional-Core/src)
include_directories(${PROJECT_SOURCE_DIR}/../Dimensional-Core/public/include)
include_directories(${PROJECT_SOURCE_DIR}/../Dimensional-Core/lib/GLM)
include_directories(${PROJECT_SOURCE_DIR}/../Dimensional-Core/lib/ENTT/single_include)
include_directories(${PROJECT_SOURCE_DIR}/../Dimensional-Core/lib/YAML/include)
include_directories(src)

# Executable
# add_executable(Portal ${APP_SOURCES})
add_executable(Portal src/PortalApp.cpp)
target_link_libraries(Portal Dimensional-Core)

if(WIN32)
    add_custom_command(TARGET Portal POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${PROJECT_SOURCE_DIR}/Assets"
            "${CMAKE_BINARY_DIR}/Assets"
        COMMENT "Copying Assets instead of symlinking on Windows"
    )
else()
    add_custom_command(TARGET Portal POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E create_symlink
            "${PROJECT_SOURCE_DIR}/Assets"
            "${CMAKE_BINARY_DIR}/Assets"
        COMMENT "Creating symlink for Assets on Linux/macOS"
    )
endif()
