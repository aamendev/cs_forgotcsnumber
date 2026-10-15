#ifndef L_FONTS_CMAP_READER_H
#define L_FONTS_CMAP_READER_H
#include "tables/cmap/cmap_subtable_types.h"
#include "types.h"
#include "utils.h"
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
namespace lina { namespace font { namespace tables{
    class cmap
    {
        public:
            struct subtable_data
            {
                std::unique_ptr<cmap_subs::format> mapping_decoder;
                u16 platform_id;
                u16 encoding_id;
                u32 language = 0;
            };
        void init(const std::string_view& view);
        u64 get_glyph_index(u64 glyph_index, i32 platform_id = -1, i32 encoding_id = -1, u32 language = 0);
        // helpers:
        void read_table_at_offset(u32 offset, u16 platform_id, u16 encoding_id);
        public:
        std::string_view m_file_view;
        std::unordered_map<u64, subtable_data> m_subtables;
    };
}}}
#endif
