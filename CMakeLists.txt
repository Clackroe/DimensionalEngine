
cmake_minimum_required(VERSION 3.5)
project(Dimensional)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(LIBRARY_OUTPUT_PATH "${CMAKE_BINARY_DIR}")
set(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}")

add_subdirectory(Dimensional-Core)
add_subdirectory(TestBed)
