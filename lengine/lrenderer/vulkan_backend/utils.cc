#include "utils.h"
#include <iostream>
#include <vulkan/vulkan_core.h>
#include "backend_manager.h"
namespace lina { namespace renderer { namespace backend { namespace utils {
    u32 find_memory_type(VkPhysicalDeviceMemoryProperties memProperties, u32 typeFilter, VkMemoryPropertyFlags flags)
    {
        for (u32 i = 0; i < memProperties.memoryTypeCount; i++) 
        {
            if ((typeFilter & (1 << i))
                    && (memProperties.memoryTypes[i].propertyFlags & flags) == flags) {
                return i;
            }
        }
        return 0;
        std::cerr << "failed to find memory type!\n";
    }
    VkCommandBuffer begin_single_time_commands(manager* man)
    {
           VkCommandBufferAllocateInfo alloc_info = 
           {
               .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
               .commandPool = man->mvk_command_pool,
               .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
               .commandBufferCount = 1,
           };
           VkCommandBuffer command_buffer;
           vkAllocateCommandBuffers(
                   man->mvkdevice, 
                   &alloc_info, &command_buffer);

           VkCommandBufferBeginInfo begin_info
           {
               .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
                   .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
           };
           vkBeginCommandBuffer(command_buffer, &begin_info);
           return command_buffer;
    }
    void end_single_time_commands(manager* man, VkCommandBuffer command_buffer)
    {
        vkEndCommandBuffer(command_buffer);
        VkSubmitInfo submit_info=
        {
            .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
            .commandBufferCount = 1,
            .pCommandBuffers = &command_buffer
        };

        vkQueueSubmit(
                man->mqueues.graphics_queue,
                1,
                &submit_info,
                VK_NULL_HANDLE);
        vkQueueWaitIdle(man->mqueues.graphics_queue);
        vkFreeCommandBuffers(
                man->mvkdevice, man->mvk_command_pool, 
                1, 
                &command_buffer);
    }

    void copy_buffer_to_img(manager* man, VkBuffer buf, VkImage img, u32 width, u32 height, u32 depth)
    {
        auto cb = begin_single_time_commands(man);
        VkBufferImageCopy img_copy
        {
            .bufferOffset = 0,
                .bufferRowLength = 0,
                .bufferImageHeight = 0,
                .imageSubresource =
                {
                    .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                    .mipLevel = 0,
                    .baseArrayLayer = 0,
                    .layerCount = 1,
                },
                .imageExtent = 
                {
                    .width = width,
                    .height = height,
                    .depth = depth
                },
        };
        vkCmdCopyBufferToImage(
                cb, buf, 
                img, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 
                1, &img_copy);
        end_single_time_commands(man, cb);
    }
}}}}
