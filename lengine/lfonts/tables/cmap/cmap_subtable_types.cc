#include "cmap_subtable_types.h"
#include <iostream>
namespace lina { namespace font { namespace tables{
    namespace cmap_subs 
    {
        u64 format0::get_glyph_index(u64 code)
        {
            if (code <= 0xff) 
            {
                return glyph_ids[code];
            }
            return 0;
        }
        u64 format2::get_glyph_index(u64 code)
        {
            u16 high_byte = code >> 8;
            u16 low_byte = code & 0xff;
            auto sub_idx = subheader_keys[high_byte] / 8;
            auto subheader = records[sub_idx];
            if (low_byte < subheader.first_code 
                    || low_byte >= subheader.first_code + subheader.entry_count)
            {
                return 0;
            }
            u16 index = low_byte - subheader.first_code;
            if (subheader.id_range_offset == 0)
            {
                return (u16)(index + subheader.id_delta);
            }
            else 
            {
                return 
                    (u16)(glyph_ids[subheader.id_range_offset] + 
                    subheader.id_delta * (glyph_ids[subheader.id_range_offset] != 0));
            }
            return 0;
        }
        u64 format4::get_glyph_index(u64 code)
        {
            u64 i = 0;
            for (i = 0; i < start_code.size(); i++)
            {
                if (start_code[i] <= code && code <= end_code[i])
                {
                    break;
                }
            }
            if (i == start_code.size()) return 0;
            if (id_range_offset[i] == 0)
            {
                return (u16)((u16)code + id_delta[i]);
            }
            else 
            {
                return 
                    (u16)(glyph_ids[id_range_offset[i]] + 
                    id_delta[i] * (glyph_ids[id_range_offset[i]] != 0));
            }
            return 0;
        }
        u64 format6::get_glyph_index(u64 code)
        {
            if (code < start_code || code >= glyph_ids.size()) return 0;
            return glyph_ids[code - start_code];
        }
        u64 format8::get_glyph_index(u64 code)
        {
            for (const auto& g : groups)
            {
                if (g.start <= code && code <= g.end)
                {
                    return g.glyph_id + (code - g.start);
                }
            }
            return 0;
        }
        u64 format10::get_glyph_index(u64 code)
        {
            if (start_code <= code && code < glyph_id.size())
            {
                return glyph_id[code - start_code];
            }
            return 0;
        }
        u64 format12::get_glyph_index(u64 code)
        {
            for (const auto& g : groups)
            {
                if (g.start <= code && code <= g.end)
                {
                    return g.glyph_start + (code - g.start);
                }
            }
            return 0;
        }
        u64 format13::get_glyph_index(u64 code)
        {
            for (const auto& g : groups)
            {
                if (g.start <= code && code <= g.end)
                {
                    return g.glyph_id;
                }
            }
            return 0;
        }
        u64 format14::get_glyph_index(u64 code)
        {
            return 0;
        }
    }
}}}
