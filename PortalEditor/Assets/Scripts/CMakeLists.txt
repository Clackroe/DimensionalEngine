cmake_minimum_required(VERSION 3.5)
project(GameApp)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Define the generated file path
set(GENERATED_FILE ${PROJECT_SOURCE_DIR}/Scripts/generated.cpp)

# Source files - using CONFIGURE_DEPENDS to ensure CMake reconfigures when files change
file(GLOB_RECURSE APP_SOURCES 
     CONFIGURE_DEPENDS
     "${PROJECT_SOURCE_DIR}/*.cpp" 
     "${PROJECT_SOURCE_DIR}/*.hpp")

# Explicitly exclude the generated file from sources to avoid duplication
list(REMOVE_ITEM APP_SOURCES "${GENERATED_FILE}")

# Include directories
include_directories(./)
include_directories(./Scripts)
include_directories(${PROJECT_SOURCE_DIR}/../../../Scripting-Core/Native/src)
include_directories(${PROJECT_SOURCE_DIR}/../../../Dimensional-Core/public/include)
include_directories(${PROJECT_SOURCE_DIR}/../../../Dimensional-Core/lib/GLM)

# ScriptCore is another CMakeProject
add_subdirectory(${PROJECT_SOURCE_DIR}/../../../Scripting-Core/Native ${CMAKE_BINARY_DIR}/Scripting-Core/Native)
set_target_properties(ScriptingCore PROPERTIES POSITION_INDEPENDENT_CODE ON)
add_subdirectory(${PROJECT_SOURCE_DIR}/../../../DReflect ${CMAKE_BINARY_DIR}/DReflect)

# Create a custom target to run the preprocessing script every time
if(WIN32)
    add_custom_target(
        generate_scripts ALL
        COMMAND ${PROJECT_SOURCE_DIR}/preprocess.bat "${PROJECT_SOURCE_DIR}/Scripts" ${GENERATED_FILE}
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "Generating ${GENERATED_FILE} on Windows"
        VERBATIM
    )
else()
    add_custom_target(
        generate_scripts ALL
        COMMAND ${CMAKE_COMMAND} -E echo "Running preprocessing script..."
        COMMAND ${CMAKE_COMMAND} -E remove -f ${GENERATED_FILE} # Remove old file
        COMMAND ${PROJECT_SOURCE_DIR}/preprocess.sh ${PROJECT_SOURCE_DIR}/Scripts ${GENERATED_FILE}
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "Generating ${GENERATED_FILE}"
        VERBATIM
    )
endif()

# Create the library
add_library(GameApp SHARED ${APP_SOURCES} ${GENERATED_FILE})
target_link_libraries(GameApp PRIVATE ScriptingCore)

# Make GameApp depend on generate_scripts and DReflect
add_dependencies(GameApp DReflect generate_scripts)

# Link include directories specifically to GameApp
target_include_directories(GameApp PRIVATE ./ PRIVATE ${PROJECT_SOURCE_DIR}/../../../Dimensional-Core/public/include)