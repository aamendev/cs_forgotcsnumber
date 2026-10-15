#include "staging_buffer.h"

namespace lina{ namespace renderer{ namespace backend { namespace buffers {
    b8 staging::init(manager* man)
    {
        m_manager = man;
        m_specs.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        m_specs.properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
            | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
        return true;
    }
    b8 staging::map_memory(u32 size)
    {
        m_specs.size = size;
        (reinterpret_cast<buffer*>(this))->create();

        vkMapMemory(
                m_manager->mvkdevice,
                m_specs.buffer_memory,
                0,
                size,
                0,
                &m_data
                );
        return true;
    }
    b8 staging::copy_memory(void* dist)
    {
        std::memcpy(m_data, dist, m_specs.size);
        vkUnmapMemory(m_manager->mvkdevice, m_specs.buffer_memory);
        return true;
    }
}}}}

