#ifndef BACKEND_INDEX_BUFFER_H
#define BACKEND_INDEX_BUFFER_H
#include "../backend_manager.h"
#include <cstring>
#include "./vertex_buffer_layout.h"
#include "./buffer.h"
namespace lina{ namespace renderer{ namespace backend { namespace buffers{
    class index: public buffer
    {
        public:
        void init(manager* device);
        void construct(
                const void* data,
                u32 size
                );

        inline u32 get_count() const;
        private:
        u32 m_count;
    };
    u32 index::get_count() const {return m_count;}
}}}}
#endif
