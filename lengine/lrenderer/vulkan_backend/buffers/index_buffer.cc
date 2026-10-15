#include "./index_buffer.h"

namespace lina{ namespace renderer{ namespace backend { namespace buffers {
    void index::init(manager* man)
    {
        m_count = 0;
        m_manager = man;
        m_specs.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
        m_specs.properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
            | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    }
    void index::construct(const void *data, u32 size)
        {
            m_count = size / 4;
            if (size > m_specs.size)
            {
                (reinterpret_cast<buffer*>(this))->destroy();
                m_specs.size = size;
                (reinterpret_cast<buffer*>(this))->create();
            }

            void* bindAddress;
            vkMapMemory(
                    m_manager->mvkdevice,
                    m_specs.buffer_memory,
                    0,
                    size,
                    0,
                    &bindAddress);
            std::memcpy(bindAddress, data, size);
            vkUnmapMemory(m_manager->mvkdevice, m_specs.buffer_memory);
        }
}}}}

