cmake_minimum_required(VERSION 3.5)
project(GameApp)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Source files
file(GLOB_RECURSE APP_SOURCES "*.cpp" "*.hpp")

# Include directories
include_directories(./)
include_directories(${PROJECT_SOURCE_DIR}/../../../Scripting-Core/Native/src)
include_directories(${PROJECT_SOURCE_DIR}/../../../Dimensional-Core/public/include)
include_directories(${PROJECT_SOURCE_DIR}/../../../Dimensional-Core/lib/GLM)

#ScriptCore is another CMakeProject
add_subdirectory(${PROJECT_SOURCE_DIR}/../../../Scripting-Core/Native ${CMAKE_BINARY_DIR}/Scripting-Core/Native)
set_target_properties(ScriptingCore PROPERTIES POSITION_INDEPENDENT_CODE ON)



add_library(GameApp SHARED ${APP_SOURCES})
# target_link_libraries(GameApp PRIVATE ScriptingCore)
target_link_libraries(GameApp PRIVATE "-Wl,--whole-archive" ScriptingCore "-Wl,--no-whole-archive")

# target_link_options(GameApp PRIVATE "-Wl,--whole-archive" ScriptingCore "-Wl,--no-whole-archive")

# target_link_libraries(GameApp ScriptingCore)

# Link include directories specifically to GameApp
target_include_directories(GameApp PRIVATE ./ PRIVATE ${PROJECT_SOURCE_DIR}/../../../Dimensional-Core/public/include)
