cmake_minimum_required(VERSION 3.17)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
project(lrenderer)

find_package(Vulkan REQUIRED)
set(LDFLAGS "-ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi")
add_library(lrenderer
    renderer.cc
    vulkan_backend/backend_manager.cc
    vulkan_backend/backend_settings.cc
    vulkan_backend/resource_manager.cc
    vulkan_backend/utils.cc
    vulkan_backend/buffers/buffer.cc
    vulkan_backend/buffers/vertex_buffer.cc
    vulkan_backend/buffers/index_buffer.cc
    vulkan_backend/buffers/staging_buffer.cc
    vulkan_backend/buffers/uniform_buffer.cc
)
add_subdirectory(lshader)

target_link_libraries(${PROJECT_NAME} GL dl vulkan lshader)
target_include_directories(lrenderer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
