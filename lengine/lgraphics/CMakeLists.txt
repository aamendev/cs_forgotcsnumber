cmake_minimum_required(VERSION 3.17)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
project(lgraphics)

add_library(lgraphics
)

target_include_directories(lgraphics PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
