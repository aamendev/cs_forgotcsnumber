#ifndef LFONTS_GLYPH_TABLE_H
#define LFONTS_GLYPH_TABLE_H
#include <lmath/types.h>
#include <string>
#include <unordered_map>
namespace lina { namespace font { namespace tables{
    class glyph_table 
    {
        public:
            struct glyph_data
            {
                struct glyph_bounds
                {
                    u64 start_col;
                    u64 glyph_width;
                    u64 start_row;
                    u64 glyph_height;
                };
                glyph_bounds texture_bounds;
                // maybe extend later
            };
            virtual void add_character_render(u64 character_index, u64 font_idx = 0) = 0;
            virtual void save_current_glyphs(std::string path) = 0;
            virtual ~glyph_table() = default;
        public:
            std::unordered_map<u64, glyph_data> m_glyph_data_registry;
    };
}}}
#endif
