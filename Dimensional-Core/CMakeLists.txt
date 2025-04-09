cmake_minimum_required(VERSION 3.5)
project(Dimensional-Core)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
# Set C++ standard
set(CMAKE_CXX_STANDARD 20)

# Source files
file(GLOB_RECURSE SOURCES "src/*.cpp")

# Include directories
include_directories(src)
include_directories(public/include)

# Vulkan
add_subdirectory(lib/vk-bootstrap)
include_directories(lib/vk-bootstrap/src)
set(VK_BOOTSTRAP_VULKAN_HEADER_DIR "lib/NVHRI/thirdparty/Vulkan-Headers")
set(VK_BOOTSTRAP_TEST OFF)



find_package(Vulkan REQUIRED)
include_directories(${Vulkan_INCLUDE_DIRS})

# NVHRI
add_subdirectory(lib/NVHRI)
include_directories(lib/NVHRI/include)
set(NVHRI_WITH_VULKAN ON)
set_target_properties(nvrhi PROPERTIES POSITION_INDEPENDENT_CODE ON)
set_target_properties(nvrhi_vk PROPERTIES POSITION_INDEPENDENT_CODE ON)


# STB
include_directories(lib/STB)

# ENTT
include_directories(lib/ENTT/single_include)

# Ensure all static libraries are compiled with -fPIC
set(CMAKE_POSITION_INDEPENDENT_CODE ON)


# ASSIMP
add_subdirectory(lib/ASSIMP)
include_directories(lib/ASSIMP/include)
# set(BUILD_SHARED_LIBS OFF)
set(ASSIMP_NO_EXPORT ON)
set(ASSIMP_INSTALL OFF)
set(ASSIMP_BUILD_ALL_EXPORTERS_BY_DEFAULT OFF)
set(ASSIMP_BUILD_TESTS OFF)
set(ASSIMP_BUILD_ASSIMP_VIEW OFF)
set_target_properties(assimp PROPERTIES POSITION_INDEPENDENT_CODE ON LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}" RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_options(assimp PRIVATE -Wno-unknown-warning-option)
    target_compile_options(assimp PRIVATE -Wno-unreachable-code-break)
    target_compile_options(assimp PRIVATE -Wno-shorten-64-to-32)

endif()


# YAML
add_subdirectory(lib/YAML)
include_directories(lib/YAML/include)
set_target_properties(yaml-cpp PROPERTIES POSITION_INDEPENDENT_CODE ON DYAML_BUILD_SHARED_LIBS OFF)

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
include_directories(lib/GLAD/include)
include_directories(lib/GLAD/include/glad)

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
target_link_libraries(Dimensional-Core glfw glad imgui spdlog yaml-cpp assimp nvrhi nvrhi_vk vk-bootstrap::vk-bootstrap ${Vulkan_LIBRARIES} )
target_compile_definitions(Dimensional-Core PRIVATE MYLIB_BUILD)

target_compile_definitions(Dimensional-Core PRIVATE DIMENSIONAL_VULKAN_RENDERER)


target_precompile_headers(Dimensional-Core PRIVATE ${PCH_HEADER})


# Install targets
install(TARGETS Dimensional-Core DESTINATION lib)
install(DIRECTORY src/ DESTINATION include FILES_MATCHING PATTERN "*.hpp")
