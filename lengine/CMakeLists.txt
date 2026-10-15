cmake_minimum_required(VERSION 3.17)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
project(lengine)

set(LDFLAGS "-ldl -lpthread")
add_library(lengine
    src/cameras/perspective.cc
    src/ecs/components/gjk.cc
    src/ecs/components/transform.cc
    src/ecs/components/colliders/mesh_collider.cc
    src/editor/editor.cc
    src/tests/tests.cc
    src/tests/core/timer_tests.cc
)
target_compile_definitions(lengine PRIVATE
    ENGINE_ROOT="${CMAKE_CURRENT_SOURCE_DIR}"
)

add_subdirectory(lrenderer)
add_subdirectory(lmesh)
add_subdirectory(lfonts)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(${PROJECT_NAME} dl lmesh lrenderer lfonts)
