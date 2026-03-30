cmake_minimum_required(VERSION 3.25)
cmake_policy(SET CMP0072 NEW)

project(BasicOpenGL VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

find_package(assimp CONFIG REQUIRED)

IF(WIN32)
    find_package(glfw3 CONFIG REQUIRED)
ELSEIF(UNIX AND NOT APPLE)
    find_package(OpenGL REQUIRED)
    find_package(glfw3 3.3 REQUIRED)
ENDIF()

IF(MSVC)
    add_compile_definitions($<$<CONFIG:Debug>:DEBUG>)
ENDIF()

if(WIN32)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
ENDIF()

configure_file(configuration/root_directory.h.in configuration/root_directory.h)

add_library(GLAD src/gl.c)
target_include_directories(GLAD PUBLIC ${CMAKE_SOURCE_DIR}/includes)
add_library(STB_IMAGE src/stb_image.cpp)
target_include_directories(STB_IMAGE PUBLIC ${CMAKE_SOURCE_DIR}/includes)

set(ENGINE_CAMERA 
    "src/Engine/Camera/Camera.cpp"
    "src/Engine/Camera/Camera.h"
)

set(ENGINE_FRAMEBUFFER
    "src/Engine/Framebuffer/Framebuffer.cpp"
    "src/Engine/Framebuffer/Framebuffer.h"
)

set(ENGINE_INSTANCES
    "src/Engine/InstancesBuffer/InstancesBuffer.cpp"
    "src/Engine/InstancesBuffer/InstancesBuffer.h"
)

set(ENGINE_MODEL
    "src/Engine/Model/Mesh.cpp"
    "src/Engine/Model/Mesh.h"
    "src/Engine/Model/Model.cpp"
    "src/Engine/Model/Model.h"
)

set(ENGINE_PARTICLE
    "src/Engine/Particles/ParticlesLit.cpp"
    "src/Engine/Particles/ParticlesLit.h"
)

set(ENGINE_PRIMITIVES
    "src/Engine/Plane/Plane.cpp"
    "src/Engine/Plane/Plane.h"
    "src/Engine/Plane/Plane2D.cpp"
    "src/Engine/Plane/Plane2D.h"
    "src/Engine/Plane/Plane3D.cpp"
    "src/Engine/Plane/Plane3D.h"
    "src/Engine/Plane/Plane3DTangent.cpp"
    "src/Engine/Plane/Plane3DTangent.h"
    "src/Engine/Sphere/Sphere.cpp"
    "src/Engine/Sphere/Sphere.h"
    "src/Engine/Sphere/SphereTangent.cpp"
    "src/Engine/Sphere/SphereTangent.h"
    "src/Engine/Cube/Cube.cpp"
    "src/Engine/Cube/Cube.h"
    "src/Engine/Vertex/Vertex.h"
)

set(ENGINE_RESOURCES 
    "src/Engine/ResourceManager/Asset.cpp"
    "src/Engine/ResourceManager/Asset.h"
    "src/Engine/ResourceManager/ResourceManager.cpp"
    "src/Engine/ResourceManager/ResourceManager.h"
)

set(ENGINE_SHADERS
    "src/Engine/Shader/Shader.cpp"
    "src/Engine/Shader/Shader.h"
    "src/Engine/Shader/ShaderProgram.cpp"
    "src/Engine/Shader/ShaderProgram.h"
    "src/Engine/Shader/ShaderProgramUniforms.cpp"
    "src/Engine/Shader/ShaderUniforms.cpp"
    "src/Engine/Shader/ShaderUniforms.h"
)

set(ENGINE_SYSTEM_UTILS
    "src/Engine/System/FileReader.cpp"
    "src/Engine/System/FileReader.h"
)

set(ENGINE_TEXTURE
    "src/Engine/Texture/Texture.cpp"
    "src/Engine/Texture/Texture.h"
)

set(ENGINE_CUBEMAP
    "src/Engine/Cubemap/Cubemap.cpp"
    "src/Engine/Cubemap/Cubemap.h"
)

set(ENGINE_UNIFORMS
    "src/Engine/UniformBlock/UniformBlock.cpp"
    "src/Engine/UniformBlock/UniformBlock.h"
)

set(UTILS_ASSETPROCESSOR
        "src/AssetProcessor/AssetProcessor.cpp"
        "src/AssetProcessor/AssetProcessorConversions.cpp"
        "src/AssetProcessor/AssetProcessor.h"
)

add_executable(AssetProcessor ${UTILS_ASSETPROCESSOR} "src/AssetProcessor/main.cpp")
target_include_directories(AssetProcessor PRIVATE ${CMAKE_SOURCE_DIR}/includes ${CMAKE_BINARY_DIR}/configuration)

add_custom_command(
    TARGET AssetProcessor
    POST_BUILD
    COMMAND $<TARGET_FILE:AssetProcessor>
            resources
            ${CMAKE_BINARY_DIR}
    WORKING_DIRECTORY $<TARGET_FILE_DIR:${PROJECT_NAME}>
    COMMENT "Regenerating Asset tags"
)

add_custom_command(
    TARGET AssetProcessor
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E rm -f "$<TARGET_FILE:AssetProcessor>"
    COMMENT "Cleaning up Tools, AssetProcessor"
)

add_custom_command(
    TARGET AssetProcessor
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
            "${CMAKE_BINARY_DIR}/asset_registry.json"
            "$<TARGET_FILE_DIR:${PROJECT_NAME}>/"
    WORKING_DIRECTORY $<TARGET_FILE_DIR:${PROJECT_NAME}>
)

set(UTILS_ASSETBUNDLER
    "src/AssetBundler/AssetBundler.cpp"
    "src/AssetBundler/AssetBundler.h"
)

add_executable(AssetBundler ${UTILS_ASSETBUNDLER} "src/AssetBundler/main.cpp")
target_include_directories(AssetBundler PRIVATE ${CMAKE_SOURCE_DIR}/includes ${CMAKE_BINARY_DIR}/configuration)

add_custom_command(
    TARGET AssetBundler
    POST_BUILD
    COMMAND $<$<CONFIG:Release>:$<TARGET_FILE:AssetBundler>>
            $<$<CONFIG:Release>:"${CMAKE_SOURCE_DIR}/resources">
            $<$<CONFIG:Release>:"$<TARGET_FILE_DIR:${PROJECT_NAME}>/resources">
    WORKING_DIRECTORY "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
    COMMAND_EXPAND_LISTS
    COMMENT "Bundling Assets (Release only)"
)

add_custom_command(
    TARGET AssetBundler
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E rm -f "$<TARGET_FILE:AssetBundler>"
    COMMENT "Cleaning up Tools, AssetBundler"
)

set(ENGINE_SOURCE 
    ${UTILS_ASSETPROCESSOR}
    ${ENGINE_SYSTEM_UTILS}
    ${ENGINE_RESOURCES}
    ${ENGINE_SHADERS}
    ${ENGINE_TEXTURE}
    ${ENGINE_CUBEMAP}
    ${ENGINE_MODEL}
    ${ENGINE_CAMERA}
    ${ENGINE_PRIMITIVES}
    ${ENGINE_FRAMEBUFFER}
    ${ENGINE_INSTANCES}
    ${ENGINE_PARTICLE}
    ${ENGINE_UNIFORMS}
    "src/Engine/Engine.h"
    "src/main.cpp"
)

add_executable(${PROJECT_NAME} ${ENGINE_SOURCE})
target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/includes" "${CMAKE_BINARY_DIR}/configuration")

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

IF(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /MP)
ENDIF()

IF(WIN32)
    target_link_libraries(${PROJECT_NAME} glfw assimp::assimp STB_IMAGE GLAD)
ELSEIF(UNIX AND NOT APPLE)
    target_link_libraries(${PROJECT_NAME} glfw assimp::assimp STB_IMAGE GLAD OpenGL::GL)
ENDIF()

IF(MSVC)
    configure_file(${CMAKE_SOURCE_DIR}/configuration/visualstudio.vcxproj.user.in ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.vcxproj.user @ONLY)
ENDIF()