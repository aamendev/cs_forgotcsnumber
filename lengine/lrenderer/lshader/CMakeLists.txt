cmake_minimum_required(VERSION 3.17)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
project(lshader)
add_library(lshader
    shader.cc
    shader_emission.cc
)

target_include_directories(lshader PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
