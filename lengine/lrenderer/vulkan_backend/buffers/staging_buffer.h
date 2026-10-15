#ifndef BACKEND_STAGING_BUFFER_H
#define BACKEND_STAGING_BUFFER_H
#include "buffer.h"
#include <cstring>
namespace lina { namespace renderer{ namespace backend { namespace buffers {
    class staging : public buffer
    {
        public:
            b8 init(manager* man);
            b8 map_memory(u32 size);
            b8 copy_memory(void* dist);
        private:
            void* m_data;
    };
}}}}
#endif
