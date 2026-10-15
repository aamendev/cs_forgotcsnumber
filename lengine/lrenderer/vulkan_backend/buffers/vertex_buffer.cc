#include "vertex_buffer.h"

namespace lina{ namespace renderer{ namespace backend { namespace buffers{
    void vertex::init(manager* deviceHandler, vblayout& layout)
    {
        m_manager = deviceHandler;
        m_layout = layout;
        m_specs.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
        m_specs.properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
            | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

        m_input_binding_description = {};
        m_input_binding_description.binding = 0;
        m_input_binding_description.stride = m_layout.get_stride();
        m_input_binding_description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;


        auto elements = m_layout.get_elements();
        m_attribute_descriptions.resize(elements.size());
        u32 offset = 0;
        for (u32 i = 0; i < m_attribute_descriptions.size(); i++)
        {
            m_attribute_descriptions[i] = {};
            m_attribute_descriptions[i].binding = 0;
            m_attribute_descriptions[i].location = elements[i].location;
            m_attribute_descriptions[i].format = static_cast<VkFormat>(elements[i].format);
            m_attribute_descriptions[i].offset = offset;
            offset +=
                elements[i].get_count(elements[i].format)
                * elements[i].get_size(elements[i].format);
        }
    }
    void vertex::construct(const void *data, u32 size)
    {
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

