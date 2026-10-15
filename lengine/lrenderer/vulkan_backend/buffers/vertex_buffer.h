#ifndef BACKEND_VERTEX_BUFFER_H
#define BACKEND_VERTEX_BUFFER_H
#include "vulkan_backend/backend_manager.h"
#include "vertex_buffer_layout.h"
#include "buffer.h"
namespace lina{ namespace renderer{ namespace backend { namespace buffers{
    class vertex: public buffer
    {
        public:
        void init(manager* device, vblayout& layout);
        void construct(
                const void* data,
                u32 size
                );

        /* Getters */
        const inline VkVertexInputBindingDescription& get_binding_description()
        {
            return m_input_binding_description;
        }

        const inline std::vector<VkVertexInputAttributeDescription>&
            get_attribute_descriptions()
            {
                return m_attribute_descriptions;
            }
        inline u32 get_stride()
        {
            return m_layout.get_stride();
        }
        /* End Getters */
        private:
        vblayout m_layout;

        std::vector<VkVertexInputAttributeDescription> m_attribute_descriptions;
        VkVertexInputBindingDescription m_input_binding_description;
    };
}}}}
#endif
