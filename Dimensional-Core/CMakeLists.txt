cmake_minimum_required(VERSION 3.5)
project(Dimensional-Core)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)

# Source files
file(GLOB_RECURSE SOURCES "src/*.cpp")

# Include directories
include_directories(src)

# UUID
include_directories(lib/UUID)

# STB
include_directories(lib/STB)

# Ensure all static libraries are compiled with -fPIC
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# GLFW
set(BUILD_SHARED_LIBS OFF)           
set(GLFW_LIBRARY_TYPE STATIC)        
set(GLFW_BUILD_EXAMPLES OFF)         
set(GLFW_BUILD_TESTS OFF)            
set(GLFW_BUILD_DOCS OFF)             

add_subdirectory(lib/GLFW)
include_directories(lib/GLFW/include)
set_target_properties(glfw PROPERTIES POSITION_INDEPENDENT_CODE ON)

# Glad
add_library(glad STATIC lib/GLAD/src/glad.c)
set_target_properties(glad PROPERTIES POSITION_INDEPENDENT_CODE ON)
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
        "lib/IMGUI/backends/imgui_impl_glfw.cpp" "lib/IMGUI/backends/imgui_impl_opengl3.cpp")

add_library(imgui STATIC ${IMGUI_SOURCES})
set_target_properties(imgui PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_include_directories(imgui PUBLIC ${IMGUI_PATH} lib/IMGUI/backends)
include_directories(lib/IMGUI)

# SPDLOG
add_subdirectory(lib/spdlog)
set_target_properties(spdlog PROPERTIES POSITION_INDEPENDENT_CODE ON)
include_directories(lib/spdlog/include)

# Precompiled Header
set(PCH_HEADER "src/core.hpp")
set_source_files_properties(${PCH_HEADER} PROPERTIES HEADER_FILE_ONLY ON)

# Shared Library
add_library(Dimensional-Core SHARED ${SOURCES})
target_link_libraries(Dimensional-Core glfw glad imgui spdlog)
target_compile_definitions(Dimensional-Core PRIVATE MYLIB_BUILD)

target_precompile_headers(Dimensional-Core PRIVATE ${PCH_HEADER})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse2 -mfpmath=sse")

# Install targets
install(TARGETS Dimensional-Core DESTINATION lib)
install(DIRECTORY src/ DESTINATION include FILES_MATCHING PATTERN "*.hpp")
