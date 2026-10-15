cmake_minimum_required(VERSION 3.17)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
project(lfonts)
add_library(lfonts
    font.cc
    tables/cff/cff_reader.cc
    tables/cff/cff_default_rasterizer.cc
    tables/cmap/cmap_reader.cc
    tables/cmap/cmap_subtable_types.cc
)

target_include_directories(lfonts PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
