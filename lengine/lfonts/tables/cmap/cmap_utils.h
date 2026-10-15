#ifndef L_FONTS_CMAP_UTILS_H
#define L_FONTS_CMAP_UTILS_H
#include <lmath/types.h>
namespace lina { namespace font { namespace tables{
    namespace cmap_utils
    {
        inline u64 get_index(u16 platform_id, u16 encoding_id, u32 language)
        {
            return  
                (platform_id << 0 * 16) & 
                (encoding_id << 1 * 16) & 
                ((u64)language << (2 * 16));
        }
    }
}}}
#endif
