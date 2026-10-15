cmake_minimum_required(VERSION 3.17)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
project(lmesh)
add_library(lmesh
    halfedge.cc
    obj_parser.cc
    mesh.cc
)

target_include_directories(lmesh PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
