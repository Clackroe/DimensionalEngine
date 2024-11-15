cmake_minimum_required(VERSION 3.5)
project(ScriptingCore)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Source files
file(GLOB_RECURSE APP_SOURCES "src/*.cpp" "src/*.hpp")

# Include directories
include_directories(src)

include_directories(${PROJECT_SOURCE_DIR}/../../Dimensional-Core/lib/GLM)
include_directories(${PROJECT_SOURCE_DIR}/../../Dimensional-Core/lib/ENTT/single_include)



add_library(ScriptingCore STATIC ${APP_SOURCES})

target_include_directories(ScriptingCore PRIVATE ./ PRIVATE ${PROJECT_SOURCE_DIR}/../../Dimensional-Core/public/include)
