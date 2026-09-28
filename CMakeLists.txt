cmake_minimum_required(VERSION 3.20)
project(CryHorrorEngine VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

# GLFW: window/input layer only.
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.4
)
FetchContent_MakeAvailable(glfw)

# Assimp: model importer. Disable everything not needed by the runtime.
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_SAMPLES OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(ASSIMP_INSTALL OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ZLIB ON CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_MINIZIP OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT ON CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ALL_EXPORTERS_BY_DEFAULT OFF CACHE BOOL "" FORCE)
set(ASSIMP_WARNINGS_AS_ERRORS OFF CACHE BOOL "" FORCE)
set(ASSIMP_NO_EXPORT ON CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ASSIMP_VIEW OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_IFC_IMPORTER OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_USD_IMPORTER OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_X3D_IMPORTER OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_COBALT_IMPORTER OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    assimp
    URL https://github.com/assimp/assimp/archive/refs/tags/v6.0.5.tar.gz
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)

add_executable(CryHorror
    src/main.cpp
    src/engine/graphics/opengl.cpp
    src/engine/graphics/shader.cpp
    src/engine/graphics/mesh.cpp
    src/engine/camera/camera.cpp
    src/engine/scene/scene.cpp
    src/engine/input/input.cpp
    src/engine/audio/audio.cpp
)

target_include_directories(CryHorror PRIVATE
    src
    third_party
)

target_link_libraries(CryHorror PRIVATE
    glfw
    assimp::assimp
)

if(WIN32)
    target_link_libraries(CryHorror PRIVATE opengl32 winmm)
endif()

if(MSVC)
    target_compile_options(CryHorror PRIVATE /W4 /permissive-)
else()
    target_compile_options(CryHorror PRIVATE -Wall -Wextra -Wpedantic)
endif()

add_custom_command(TARGET CryHorror POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/assets
            $<TARGET_FILE_DIR:CryHorror>/assets
)
