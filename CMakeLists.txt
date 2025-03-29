cmake_minimum_required(VERSION 3.5)
set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)
project(Dimensional)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(LIBRARY_OUTPUT_PATH "${CMAKE_BINARY_DIR}")
set(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}")

set(CMAKE_BUILD_PARALLEL_LEVEL 12)

add_subdirectory(Dimensional-Core)
add_subdirectory(PortalEditor)
# add_subdirectory(DReflect)
add_subdirectory(PortalEditor/Assets/Scripts)
add_subdirectory(TestBed)
