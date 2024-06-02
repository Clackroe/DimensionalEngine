cmake_minimum_required(VERSION 3.5)
project(Voxler)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)

# Source files
file(GLOB_RECURSE SOURCES "src/*.cpp")

# Include directories
include_directories(src)

# GLFW
add_subdirectory(lib/GLFW)
include_directories(lib/GLFW/include)

# Glad
add_library(glad STATIC lib/GLAD/src/glad.c)
include_directories(lib/GLAD/include/GLAD)
include_directories(lib/GLAD/include/KHR)

# GLM
include_directories(lib/GLM)

# ImGui
set(IMGUI_PATH  "lib/IMGUI/")

file(GLOB IMGUI_SOURCES
        "lib/IMGUI/*.h"
        "lib/IMGUI/*.cpp"
        "lib/IMGUI/backends/imgui_impl_glfw.h"
        "lib/IMGUI/backends/imgui_impl_opengl3.h"
        "lib/IMGUI/backends/imgui_impl_glfw.cpp"
        "lib/IMGUI/backends/imgui_impl_opengl3.cpp")

add_library(imgui ${IMGUI_SOURCES})
target_include_directories(imgui PUBLIC ${IMGUI_PATH} lib/IMGUI/backends)
include_directories(lib/IMGUI)


# SPDLOG
include_directories(lib/spdlog/include)

# Precompiled Header
set(PCH_HEADER "src/core.hpp")
set_source_files_properties(${PCH_HEADER} PROPERTIES HEADER_FILE_ONLY ON)



# Executable
add_executable(Voxler-Core ${SOURCES})
target_link_libraries(Voxler-Core glfw glad imgui spdlog)

target_precompile_headers(Voxler-Core PRIVATE ${PCH_HEADER})
