cmake_minimum_required(VERSION 4.0)
cmake_policy(SET CMP0072 NEW)

project(BasicOpenGL VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

find_package(glm CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(assimp CONFIG REQUIRED)

IF(MSVC)
    add_compile_definitions($<$<CONFIG:Debug>:DEBUG>)
ENDIF()

IF(WIN32)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
ENDIF(WIN32)

if(WIN32)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif(WIN32)

configure_file(configuration/root_directory.h.in configuration/root_directory.h)
include_directories(${CMAKE_BINARY_DIR}/configuration)

add_library(STB_IMAGE src/stb_image.cpp)
add_library(GLAD src/gl.c)

file(GLOB SOURCE
        "src/Engine/**/*.h"
        "src/Engine/**/*.cpp"
        "src/Engine/*.h"
        "src/main.cpp"
)
    
set(ASSETPROCESSOR "AssetProcessor")
file(GLOB ASSETPROCESSORLIBRARY
        "src/AssetProcessor/AssetProcessor.cpp"
        "src/AssetProcessor/AssetProcessorConversions.cpp"
        "src/AssetProcessor/AssetProcessor.h"
)
add_library(AssetProcessorCore ${ASSETPROCESSORLIBRARY})
file(GLOB ASSETPROCESSORSOURCE
        "src/AssetProcessor/main.cpp"
)
add_executable(${ASSETPROCESSOR} ${ASSETPROCESSORSOURCE})
include_directories(${ASSETPROCESSOR} ${CMAKE_SOURCE_DIR}/includes ${CMAKE_BINARY_DIR}/configuration)
target_link_libraries(${ASSETPROCESSOR} AssetProcessorCore)

set(ASSETBUNDLER "AssetBundler")
file(GLOB ASSETBUNDLERSOURCE
    "src/AssetBundler/*.h"
    "src/AssetBundler/*.cpp"
)
add_executable(${ASSETBUNDLER} ${ASSETBUNDLERSOURCE})
include_directories(${ASSETBUNDLER} ${CMAKE_SOURCE_DIR}/includes ${CMAKE_BINARY_DIR}/configuration)

set_target_properties(${ASSETPROCESSOR} ${ASSETBUNDLER} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/tools"
)

add_executable(${PROJECT_NAME} ${COMMON} ${SOURCE})
add_dependencies(${PROJECT_NAME} ${ASSETPROCESSOR} ${ASSETBUNDLER})

add_custom_command(
    TARGET ${PROJECT_NAME}
    PRE_BUILD
    COMMAND $<TARGET_FILE:${ASSETPROCESSOR}>
            resources
            ${CMAKE_BINARY_DIR}
    WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
)

add_custom_command(
    TARGET ${PROJECT_NAME}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
            "${CMAKE_BINARY_DIR}/asset_registry.json"
            "$<TARGET_FILE_DIR:${PROJECT_NAME}>/"
    WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
)

add_custom_command(
    TARGET ${PROJECT_NAME}
    POST_BUILD
    COMMAND $<$<CONFIG:Debug>:${CMAKE_COMMAND}> 
            $<$<CONFIG:Debug>:-E> 
            $<$<CONFIG:Debug>:copy_directory> 
            $<$<CONFIG:Debug>:${CMAKE_SOURCE_DIR}/resources> 
            $<$<CONFIG:Debug>:$<TARGET_FILE_DIR:${PROJECT_NAME}>/resources>
    COMMAND_EXPAND_LISTS
    COMMENT "Copying resources (Debug only)"
)

add_custom_command(
    TARGET ${PROJECT_NAME}
    POST_BUILD
    COMMAND $<$<CONFIG:Release>:$<TARGET_FILE:${ASSETBUNDLER}>>
    WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
    COMMAND_EXPAND_LISTS
    COMMENT "Bundling Assets (Release only)"
)

add_custom_command(
    TARGET ${PROJECT_NAME}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E rm -f "$<TARGET_FILE:${ASSETPROCESSOR}>"
    COMMAND ${CMAKE_COMMAND} -E rm -f "$<TARGET_FILE:${ASSETBUNDLER}>"
    COMMENT "Cleaning up build-time helper tools..."
)

IF(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /std:c++20 /MP)
    target_link_options(${PROJECT_NAME} PUBLIC /ignore:4099)
ENDIF(MSVC)

IF(WIN32)
    target_link_libraries(${PROJECT_NAME} glfw assimp::assimp glm::glm STB_IMAGE GLAD AssetProcessorCore)
ENDIF(WIN32)

IF(MSVC)
    configure_file(${CMAKE_SOURCE_DIR}/configuration/visualstudio.vcxproj.user.in ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.vcxproj.user @ONLY)
ENDIF(MSVC)

include_directories(${CMAKE_SOURCE_DIR}/includes)
include_directories(${CMAKE_SOURCE_DIR}/src)