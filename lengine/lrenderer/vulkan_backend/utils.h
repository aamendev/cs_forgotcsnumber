#ifndef BACKEND_UTILS_H
#define BACKEND_UTILS_H
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>
#include "types.h"
namespace lina { namespace renderer { namespace backend { 
    class manager;
    namespace utils {
        u32 find_memory_type(VkPhysicalDeviceMemoryProperties props, u32 typeFilter, VkMemoryPropertyFlags flags);
        VkCommandBuffer begin_single_time_commands(manager* man);
        void end_single_time_commands(manager* man, VkCommandBuffer command_buffer);
        void copy_buffer_to_img(manager* man, VkBuffer buf, VkImage img, u32 width, u32 height = 1, u32 depth = 1);
    }}}}
#endif
