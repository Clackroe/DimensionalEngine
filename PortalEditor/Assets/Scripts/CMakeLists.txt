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
include_directories(./Scripts)
include_directories(${PROJECT_SOURCE_DIR}/../../../Scripting-Core/Native/src)
include_directories(${PROJECT_SOURCE_DIR}/../../../Dimensional-Core/public/include)
include_directories(${PROJECT_SOURCE_DIR}/../../../Dimensional-Core/lib/GLM)

#ScriptCore is another CMakeProject
add_subdirectory(${PROJECT_SOURCE_DIR}/../../../Scripting-Core/Native ${CMAKE_BINARY_DIR}/Scripting-Core/Native)
set_target_properties(ScriptingCore PROPERTIES POSITION_INDEPENDENT_CODE ON)


add_subdirectory(${PROJECT_SOURCE_DIR}/../../../DReflect ${CMAKE_BINARY_DIR}/DReflect)



set(GENERATED_FILE ${PROJECT_SOURCE_DIR}/Scripts/generated.cpp)

if(WIN32)
    add_custom_command(
        OUTPUT ${GENERATED_FILE}
        COMMAND ${PROJECT_SOURCE_DIR}/preprocess.bat ${GENERATED_FILE}
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        DEPENDS ${PROJECT_SOURCE_DIR}/preprocess.bat
        COMMENT "Generating ${GENERATED_FILE} on Windows"
        VERBATIM
    )
else()
    # Custom command to generate the file
add_custom_command(
    OUTPUT ${GENERATED_FILE}
    COMMAND ${CMAKE_COMMAND} -E echo "Running preprocessing script..."
    COMMAND ${CMAKE_COMMAND} -E remove -f ${GENERATED_FILE} # Remove old file
    COMMAND ${PROJECT_SOURCE_DIR}/preprocess.sh ${GENERATED_FILE}
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    DEPENDS ${PROJECT_SOURCE_DIR}/preprocess.sh
    COMMENT "Generating ${GENERATED_FILE}"
    VERBATIM
)

endif()


# Ensure the generated file is included in the build
add_library(GameApp SHARED ${APP_SOURCES} ${GENERATED_FILE})

target_link_libraries(GameApp PRIVATE ScriptingCore)

add_dependencies(GameApp DReflect)

if(WIN32)
    add_custom_command(TARGET GameApp PRE_BUILD
    COMMAND ${PROJECT_SOURCE_DIR}/preprocess.bat)
else()
    add_custom_command(TARGET GameApp PRE_BUILD
    COMMAND ${PROJECT_SOURCE_DIR}/preprocess.sh)
endif()



# Link include directories specifically to GameApp
target_include_directories(GameApp PRIVATE ./ PRIVATE ${PROJECT_SOURCE_DIR}/../../../Dimensional-Core/public/include)
