#ifndef BACKEND_IMAGE_H
#define BACKEND_IMAGE_H
#include <vulkan/vulkan_core.h>
#include "backend_manager.h"
#include "vulkan_backend/utils.h"
#include "types.h"
namespace lina{ namespace renderer{ namespace backend{
    class image
    {
        public:
            void create(
                    VkFormat format, 
                    VkImageUsageFlagBits usage_flags, 
                    manager* man,
                    u32 width,
                    u32 height = 1,
                    u32 depth = 1);
            void transition_layout(VkImageLayout new_layout);
        public:
            VkImage m_image;
            VkImageView m_view;
            VkDeviceMemory m_memory;
            manager* m_manager;
        protected:
            VkImageUsageFlagBits m_usage_flags;
            VkFormat m_format;
        public:
            u32 m_miplevels = 1;
            u32 m_array_layers = 1;
            VkMemoryPropertyFlags m_memory_flags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
            VkImageLayout m_initial_layout = VK_IMAGE_LAYOUT_UNDEFINED;
            VkSharingMode m_sharing_mode = VK_SHARING_MODE_EXCLUSIVE;
            VkImageTiling m_tiling = VK_IMAGE_TILING_OPTIMAL;
            VkImageCreateFlagBits m_create_flgs = (VkImageCreateFlagBits)0;
            VkImageType m_type = VK_IMAGE_TYPE_2D;
            VkSampleCountFlagBits m_sample_count = VK_SAMPLE_COUNT_1_BIT;
    };
}}}
#endif
