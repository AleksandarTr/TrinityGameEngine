cmake_minimum_required(VERSION 3.27)
project(TrinityEngine)

set(CMAKE_CXX_STANDARD 17)
set(dir ${CMAKE_CURRENT_SOURCE_DIR})
include_directories("include")
include_directories("src")

add_executable(TrinityEngine main.cpp glad.c
        Shader.cpp
        Shader.h
        VertexArrayObject.cpp
        VertexArrayObject.h
        VertexBufferObject.cpp
        VertexBufferObject.h
        ElementBufferObject.cpp
        ElementBufferObject.h
        Texture.cpp
        Texture.h
        Object.cpp
        Object.h
        Camera.cpp
        Camera.h)

add_library(glfw STATIC IMPORTED)
set_target_properties(glfw PROPERTIES
        IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/src/lib-mingw-w64/libglfw3.a"
        INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(TrinityEngine glfw)