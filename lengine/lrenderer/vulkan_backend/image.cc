#include "image.h"
#include "utils.h"
#include <vulkan/vulkan_core.h>

namespace lina{ namespace renderer{ namespace backend{
    void image::create(
            VkFormat format, 
            VkImageUsageFlagBits usage_flags, 
            manager* man,
            u32 width,
            u32 height,
            u32 depth)
    {
        m_manager = man;
        m_format = format;
        m_usage_flags = usage_flags;
        VkExtent3D ext{width, height, depth};
        VkImageCreateInfo info =
        {
            .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
            .flags = m_usage_flags,
            .imageType = m_type,
            .format = m_format,
            .extent = ext,
            .mipLevels = m_miplevels,
            .arrayLayers = m_array_layers,
            .samples = m_sample_count,
            .tiling = m_tiling,
            .usage = m_usage_flags,
            .initialLayout = m_initial_layout,
        };

        vkCreateImage(
                m_manager->mvkdevice, 
                &info, 
                nullptr, &m_image);

        VkMemoryRequirements memRequirements;
        vkGetImageMemoryRequirements(
                m_manager->mvkdevice,
                m_image, 
                &memRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex =
            utils::find_memory_type(m_manager->mpd.memory_properties,
                    memRequirements.memoryTypeBits,
                    m_memory_flags
                    );
        
        vkAllocateMemory(
                m_manager->mvkdevice, 
               &allocInfo, nullptr, &m_memory);
        vkBindImageMemory(
                m_manager->mvkdevice, 
                m_image,
                m_memory,
                0);

        VkImageViewCreateInfo view_info
        {
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .image = m_image,
            .viewType = (VkImageViewType)m_type,
            .format = m_format,
            .subresourceRange = 
            {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 1,
            },
        };
        vkCreateImageView(
                m_manager->mvkdevice, 
                &view_info, nullptr, &m_view);
    }
    void image::transition_layout(VkImageLayout new_layout)
    {
        auto cb = utils::begin_single_time_commands(m_manager);
        VkImageMemoryBarrier barrier
        {
            .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
                .srcAccessMask = 0,
                .dstAccessMask = 0,
                .oldLayout = m_initial_layout,
                .newLayout = new_layout,
                .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .image = m_image,
            .subresourceRange=
            {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 1
            },
        };
        VkPipelineStageFlags src;
        VkPipelineStageFlags dst;
        switch (m_initial_layout)
        {
            case VK_IMAGE_LAYOUT_UNDEFINED:
            {
                src = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            }
            break;
            case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
            {
                barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
                src = VK_PIPELINE_STAGE_TRANSFER_BIT;
            }
            break;
            default: {}
        }
        switch (new_layout)
        {
            case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
            {
                dst = VK_PIPELINE_STAGE_TRANSFER_BIT;
                barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            }
            break;
            case VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL:
            {
                dst = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
                barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
            }
            break;
            default: {}
        }
        vkCmdPipelineBarrier(
                cb, 
                src, dst, 
                0, 0,  nullptr, 0, nullptr, 
                1, &barrier);
        utils::end_single_time_commands(m_manager, cb);
    }

}}}
