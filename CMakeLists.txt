cmake_minimum_required(VERSION 3.20)
project(CryHorrorEngine VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.4
)
FetchContent_MakeAvailable(glfw)

add_executable(CryHorror
    src/main.cpp
    src/engine/graphics/opengl.cpp
    src/engine/graphics/shader.cpp
    src/engine/input/input.cpp
)

target_include_directories(CryHorror PRIVATE src)
target_link_libraries(CryHorror PRIVATE glfw)

if(WIN32)
    target_link_libraries(CryHorror PRIVATE opengl32)
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