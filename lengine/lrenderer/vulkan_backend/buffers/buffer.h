#ifndef BACKEND_BUFFER_H
#define BACKEND_BUFFER_H
#include <vulkan/vulkan_core.h>
#include "types.h"
#include "vulkan_backend/backend_manager.h"
#include "vulkan_backend/utils.h"
#include <cstring>
namespace lina{ namespace renderer{ namespace backend{
    class buffer
    {
        struct specs
        {
            u32 count = 1;
            VkDeviceSize size = 0;
            u32 unaligned_size = 0;
            VkBufferUsageFlags usage;
            VkMemoryPropertyFlags properties;
            VkBuffer buffer = VK_NULL_HANDLE;
            VkDeviceMemory buffer_memory = VK_NULL_HANDLE;
        };
        public:
            virtual void create();
            inline VkDeviceSize get_size() const{ return m_specs.size; }
            inline VkBuffer& get_buffer() { return m_specs.buffer; }
            inline u32 get_count() const {return m_specs.count;}
            void destroy();
        protected:
            manager* m_manager;
            specs m_specs;

    };
}}}
#endif
