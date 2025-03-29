cmake_minimum_required(VERSION 3.5)
project(DimensionalReflect)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Source files
file(GLOB_RECURSE APP_SOURCES "src/*.cpp")

# Include directories
include_directories(src)
include_directories(${PROJECT_SOURCE_DIR}/../Dimensional-Core/public/include)

# Define executable
add_executable(DReflect ${APP_SOURCES})

# Ensure correct linking (add dependencies here if needed)
target_link_libraries(DReflect)

# Define default permissions for installed executables
set(PROGRAM_PERMISSIONS_ALL
    OWNER_READ OWNER_WRITE OWNER_EXECUTE
    GROUP_READ GROUP_EXECUTE
    WORLD_READ WORLD_EXECUTE)

# Install target with the correct permissions
install(TARGETS DReflect RUNTIME DESTINATION bin PERMISSIONS ${PROGRAM_PERMISSIONS_DEFAULT})
