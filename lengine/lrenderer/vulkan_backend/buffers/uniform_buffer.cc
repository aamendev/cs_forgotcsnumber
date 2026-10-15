#include "uniform_buffer.h"
#include <cstring>
namespace lina { namespace renderer { namespace backend { namespace buffers {
    void uniform::init(manager* mang)
    {        
        m_manager = mang;
        m_specs.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        m_specs.properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
    }
    void uniform::construct(u32 size, u32 count)
    {
        m_specs.unaligned_size = size;
        auto minAlign = m_manager->mpd.properties.limits.minUniformBufferOffsetAlignment;
        m_specs.size = size;
        m_unaligned_size = size;
        if (minAlign > 0)
        {
            m_specs.size = (size + minAlign - 1) & ~(minAlign - 1);
        }
        m_specs.count = count;
        (reinterpret_cast<buffer*>(this))->create();

        vkMapMemory(
                m_manager->mvkdevice,
                m_specs.buffer_memory,
                0,
                m_specs.size * count,
                0,
                &m_data);

    }
    void uniform::write_data(void* data, i32 slot)
    {
        std::memcpy((void*)((u8*)m_data + slot * m_specs.size), data, 
                m_unaligned_size);
        VkMappedMemoryRange flushMem
        {
            .sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE,
                .memory = m_specs.buffer_memory,
                .offset = slot * m_specs.size,
                .size = m_specs.size
        };

        vkFlushMappedMemoryRanges(m_manager->mvkdevice, 
                1,
                &flushMem);

    }
}}}}
