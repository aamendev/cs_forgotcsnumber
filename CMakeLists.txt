cmake_minimum_required(VERSION 3.17)
project(lrun)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(LDFLAGS "-ldl")
set(GCCCompilerFlags "-std=c++23")
set(CMAKE_BUILD_TYPE Debug)

FILE(GLOB SRC
    "main.cc"
    "./*.cc"
    "./layers/*.cc"
    )

add_executable(${PROJECT_NAME} ${SRC})
add_subdirectory(lengine)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(${PROJECT_NAME} dl lengine glfw)
