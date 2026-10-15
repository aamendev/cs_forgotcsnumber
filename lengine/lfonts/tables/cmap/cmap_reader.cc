#include "cmap_reader.h"
#include "tables/cmap/cmap_subtable_types.h"
#include "tables/cmap/cmap_utils.h"
#include "utils.h"
#include <algorithm>
#include <map>
#include <memory>
#include <queue>
namespace lina { namespace font { namespace tables{
    void cmap::init(const std::string_view& str)
    {
        m_file_view = str;
        u64 curr_offset = 0;
        u16 table_count = 0;
        curr_offset = utils::convert_bytes(str, curr_offset, &table_count);
        table_count = 0;
        curr_offset = utils::convert_bytes(str, curr_offset, &table_count);
        for (u16 i = 0; i < table_count; i++)
        {
            u16 pid = 0;
            u16 eid = 0;
            u32 offset = 0;
            curr_offset = utils::convert_bytes(str, curr_offset, &pid);
            curr_offset = utils::convert_bytes(str, curr_offset, &eid);
            curr_offset = utils::convert_bytes(str, curr_offset, &offset);
            read_table_at_offset(offset, pid, eid);
        }
    }
    u64 cmap::get_glyph_index(u64 glyph_index, i32 platform_id, i32 encoding_id, u32 language)
    {
        u64 idx = 0;
        if (platform_id > -1 && encoding_id > -1)
        {
            idx = cmap_utils::get_index(platform_id, encoding_id, language);
            if (m_subtables.contains(idx))
            {
                return m_subtables[idx].mapping_decoder->get_glyph_index(glyph_index);
            }
        }
        else 
        {
            std::priority_queue<std::pair<u64, u8>, 
                std::vector<std::pair<u64, u8>>,
                cmap_subs::subtable_comparator> sorted_data;
            for (const auto& [key, value] : m_subtables)
            {
                auto format_num = value.mapping_decoder->get_format_number();
                u8 local_score = 
                    (
                    (format_num == 12) * 10 + 
                    (format_num == 4) * 9 + 
                    (format_num == 13) * 8 + 
                    (format_num == 14) * 7 + 
                    (format_num == 10) * 6 + 
                    (format_num == 6) * 5 + 
                    (format_num == 2) * 4 + 
                    (format_num == 0) * 3 + 
                    (format_num == 8 || format_num == 13) * 2);
                sorted_data.push({key, local_score});
            }
            while (!sorted_data.empty())
            {
                idx = sorted_data.top().first;
                u64 id =  m_subtables[idx].mapping_decoder->get_glyph_index(glyph_index);
                if (id != 0)
                {
                    return id;
                }
                sorted_data.pop();
            }
        }
        return 0;
    }
    void cmap::read_table_at_offset(u32 offset, u16 platform_id, u16 encoding_id)
    {
        u32 old_offset = offset;
        u16 format = 0;
        u16 language = 0;
        u16 length = 0;

        offset = utils::convert_bytes(m_file_view, offset, &format);
        if (format < 14)
        {
            offset = utils::convert_bytes(m_file_view, offset, &length);
            if (format < 8)
            {
                offset = utils::convert_bytes(m_file_view, offset, &language);
            }
        }
        subtable_data curr_subtable;
        switch (format)
        {
            case 0:
                {
                    curr_subtable.mapping_decoder = std::make_unique<cmap_subs::format0>();
                    auto* mapping_pointer = 
                        dynamic_cast<cmap_subs::format0*>(curr_subtable.mapping_decoder.get());
                    utils::convert_bytes(
                            m_file_view, 
                            offset, 
                            mapping_pointer->glyph_ids.data(), 
                            mapping_pointer->glyph_ids.size());
                    curr_subtable.language = language;
                }
                break;
            case 2:
                {
                    curr_subtable.mapping_decoder = std::make_unique<cmap_subs::format2>();
                    auto* mapping_pointer = 
                        dynamic_cast<cmap_subs::format2*>(curr_subtable.mapping_decoder.get());
                    offset = utils::convert_bytes(
                            m_file_view, 
                            offset, 
                            mapping_pointer->subheader_keys.data(),
                            mapping_pointer->subheader_keys.size());
                    u16 subheaders_count = 
                        (*std::max_element(
                                           mapping_pointer->subheader_keys.begin(), 
                                           mapping_pointer->subheader_keys.end())) / 8 + 1;
                    u64 glyph_start_idx = offset + (subheaders_count * 4 * sizeof(u16));
                    mapping_pointer->records.resize(subheaders_count);
                    for (u16 i = 0; i < subheaders_count; i++)
                    {
                        offset = utils::convert_bytes(
                                m_file_view, 
                                offset, 
                                &mapping_pointer->records[i].first_code);
                        offset = utils::convert_bytes(
                                m_file_view, 
                                offset, 
                                &mapping_pointer->records[i].entry_count);
                        offset = utils::convert_bytes(
                                m_file_view, 
                                offset, 
                                &mapping_pointer->records[i].id_delta);
                        offset = utils::convert_bytes(
                                m_file_view, 
                                offset, 
                                &mapping_pointer->records[i].id_range_offset);
                        if (mapping_pointer->records[i].id_range_offset != 0)
                        {
                            u64 local_offset = offset + mapping_pointer->records[i].id_range_offset - 2;
                            mapping_pointer->records[i].id_range_offset = 
                                (local_offset - glyph_start_idx)/2;
                        }
                    }
                    u64 sz = (length - (offset - old_offset))/2;
                    mapping_pointer->glyph_ids.resize(sz);
                    offset = utils::convert_bytes(
                            m_file_view, 
                            offset, 
                            mapping_pointer->glyph_ids.data(),
                            mapping_pointer->glyph_ids.size()
                            );
                    curr_subtable.language = language;
                }
                break;
            case 4:
                {
                    curr_subtable.mapping_decoder = std::make_unique<cmap_subs::format4>();
                    auto* mapping_pointer = 
                        dynamic_cast<cmap_subs::format4*>(curr_subtable.mapping_decoder.get());
                    u16 seg_count = 0;
                    offset = utils::convert_bytes(
                            m_file_view, 
                            offset, 
                            &seg_count); 
                    seg_count/=2;

                    mapping_pointer->start_code.resize(seg_count, 0);
                    mapping_pointer->end_code.resize(seg_count , 0);
                    mapping_pointer->id_delta.resize(seg_count, 0);
                    mapping_pointer->id_range_offset.resize(seg_count, 0);

                    offset += 3 * sizeof(u16); // skip search hints
                                 //

                    offset = utils::convert_bytes(
                            m_file_view, 
                            offset, 
                            mapping_pointer->end_code.data(),
                            mapping_pointer->end_code.size());

                    offset += sizeof(u16); // skip padding
                                           //
                    offset = utils::convert_bytes(
                            m_file_view, 
                            offset, 
                            mapping_pointer->start_code.data(),
                            mapping_pointer->start_code.size());

                    offset = utils::convert_bytes(
                            m_file_view, 
                            offset, 
                            mapping_pointer->id_delta.data(),
                            mapping_pointer->id_delta.size());

                    offset = utils::convert_bytes(
                            m_file_view, 
                            offset, 
                            mapping_pointer->id_range_offset.data(),
                            mapping_pointer->id_range_offset.size());

                    for (u16 i = 0; i < seg_count; i++)
                    {
                        if (mapping_pointer->id_range_offset[i] != 0)
                        {
                            mapping_pointer->id_range_offset[i] -= (2 * (seg_count - i));
                            mapping_pointer->id_range_offset[i] /= 2;
                        }
                    }
                    u64 sz = (length - (offset - old_offset))/2;
                    mapping_pointer->glyph_ids.resize(sz);
                    offset = utils::convert_bytes(
                            m_file_view, 
                            offset, 
                            mapping_pointer->glyph_ids.data(),
                            mapping_pointer->glyph_ids.size()
                            );
                    curr_subtable.language = language;
                }
                break;
            case 6:
                {
                    curr_subtable.mapping_decoder = std::make_unique<cmap_subs::format6>();
                    auto* mapping_pointer = 
                        dynamic_cast<cmap_subs::format6*>(curr_subtable.mapping_decoder.get());
                    offset = utils::convert_bytes(
                            m_file_view, 
                            offset, 
                            &mapping_pointer->start_code); 
                    u16 entry_count = 0;
                    offset = utils::convert_bytes(
                            m_file_view, 
                            offset, 
                            &entry_count); 

                    mapping_pointer->glyph_ids.resize(entry_count);
                    offset = utils::convert_bytes(
                            m_file_view, 
                            offset, 
                            mapping_pointer->glyph_ids.data(),
                            mapping_pointer->glyph_ids.size()
                            );
                    curr_subtable.language = language;
                }
                break;
            case 8:
                {
                    curr_subtable.mapping_decoder = std::make_unique<cmap_subs::format8>();
                    auto* mapping_pointer = 
                        dynamic_cast<cmap_subs::format8*>(curr_subtable.mapping_decoder.get());

                    u32 length = 0;
                    u32 language = 0;
                    u32 num_groups = 0;
                    offset = utils::convert_bytes(
                            m_file_view, 
                            offset, 
                            &length);
                    offset = utils::convert_bytes(
                            m_file_view, 
                            offset, 
                            &language);
                    offset = utils::convert_bytes(
                            m_file_view, 
                            offset, 
                            &num_groups);

                    mapping_pointer->groups.resize(num_groups);

                    for (u32 i = 0; i < num_groups; i++)
                    {
                        offset = utils::convert_bytes(
                                m_file_view, 
                                offset, 
                                &mapping_pointer->groups[i].start);
                        offset = utils::convert_bytes(
                                m_file_view, 
                                offset, 
                                &mapping_pointer->groups[i].end);
                        offset = utils::convert_bytes(
                                m_file_view, 
                                offset, 
                                &mapping_pointer->groups[i].glyph_id);
                    }

                    curr_subtable.language = language;
                }
                break;
            case 10:
                {
                    curr_subtable.mapping_decoder = std::make_unique<cmap_subs::format10>();
                    auto* mapping_pointer = 
                        dynamic_cast<cmap_subs::format10*>(curr_subtable.mapping_decoder.get());

                    u32 length = 0;
                    u32 language = 0;
                    u32 num_chars = 0;
                    offset = utils::convert_bytes(
                            m_file_view, 
                            offset, 
                            &length);
                    offset = utils::convert_bytes(
                            m_file_view, 
                            offset, 
                            &language);
                    offset = utils::convert_bytes(
                            m_file_view, 
                            offset, 
                            &mapping_pointer->start_code);
                    offset = utils::convert_bytes(
                            m_file_view, 
                            offset, 
                            &num_chars);

                    mapping_pointer->glyph_id.resize(num_chars);

                    offset = utils::convert_bytes(
                            m_file_view, 
                            offset, 
                            mapping_pointer->glyph_id.data(),
                            mapping_pointer->glyph_id.size()
                            );

                    curr_subtable.language = language;
                }
                break;
            case 12:
                {
                    curr_subtable.mapping_decoder = std::make_unique<cmap_subs::format12>();
                    auto* mapping_pointer = 
                        dynamic_cast<cmap_subs::format12*>(curr_subtable.mapping_decoder.get());

                    u32 length = 0;
                    u32 language = 0;
                    u32 num_groups = 0;
                    offset = utils::convert_bytes(
                            m_file_view, 
                            offset, 
                            &length);
                    offset = utils::convert_bytes(
                            m_file_view, 
                            offset, 
                            &language);
                    offset = utils::convert_bytes(
                            m_file_view, 
                            offset, 
                            &num_groups);

                    mapping_pointer->groups.resize(num_groups);

                    for (u32 i = 0; i < num_groups; i++)
                    {
                        offset = utils::convert_bytes(
                                m_file_view, 
                                offset, 
                                &mapping_pointer->groups[i].start);
                        offset = utils::convert_bytes(
                                m_file_view, 
                                offset, 
                                &mapping_pointer->groups[i].end);
                        offset = utils::convert_bytes(
                                m_file_view, 
                                offset, 
                                &mapping_pointer->groups[i].glyph_start);
                    }

                    curr_subtable.language = language;
                }
                break;
            case 13:
                {
                    curr_subtable.mapping_decoder = std::make_unique<cmap_subs::format13>();
                    auto* mapping_pointer = 
                        dynamic_cast<cmap_subs::format13*>(curr_subtable.mapping_decoder.get());

                    u32 length = 0;
                    u32 language = 0;
                    u32 num_groups = 0;
                    offset = utils::convert_bytes(
                            m_file_view, 
                            offset, 
                            &length);
                    offset = utils::convert_bytes(
                            m_file_view, 
                            offset, 
                            &language);
                    offset = utils::convert_bytes(
                            m_file_view, 
                            offset, 
                            &num_groups);

                    mapping_pointer->groups.resize(num_groups);

                    for (u32 i = 0; i < num_groups; i++)
                    {
                        offset = utils::convert_bytes(
                                m_file_view, 
                                offset, 
                                &mapping_pointer->groups[i].start);
                        offset = utils::convert_bytes(
                                m_file_view, 
                                offset, 
                                &mapping_pointer->groups[i].end);
                        offset = utils::convert_bytes(
                                m_file_view, 
                                offset, 
                                &mapping_pointer->groups[i].glyph_id);
                    }

                    curr_subtable.language = language;
                }
                break;
            case 14:
                {
                    curr_subtable.mapping_decoder = std::make_unique<cmap_subs::format14>();
                    auto* mapping_pointer = 
                        dynamic_cast<cmap_subs::format14*>(curr_subtable.mapping_decoder.get());

                    u32 length = 0;
                    u32 num_var_selectors = 0;
                    offset = utils::convert_bytes(
                            m_file_view, 
                            offset, 
                            &length);
                    offset = utils::convert_bytes(
                            m_file_view, 
                            offset, 
                            &num_var_selectors);

                    mapping_pointer->selectors.resize(num_var_selectors);
                    for (u32 i = 0; i < num_var_selectors; i++)
                    {
                        auto& curr_var = mapping_pointer->selectors[i];
                        std::array<u8, 3> local_var_arr;
                        offset = utils::convert_bytes(
                                m_file_view, 
                                offset, 
                                local_var_arr.data(),
                                local_var_arr.size());
                        u32 local_var = 0;
                        for (auto j = 0; j < 3; j++)
                        {
                            local_var &= ((u32)local_var_arr[j] << (j));
                        }
                        curr_var.selector = local_var;
                        u32 range_size = 0;
                        offset = utils::convert_bytes(
                                m_file_view, 
                                offset, 
                                &curr_var.default_offset);
                        offset = utils::convert_bytes(
                                m_file_view, 
                                offset, 
                                &curr_var.nondefault_offset);
                        if (curr_var.default_offset == 0)
                        {
                            curr_var.is_default = false;
                            u64 local_offset = curr_var.nondefault_offset;
                            local_offset = utils::convert_bytes(
                                    m_file_view, 
                                    local_offset, 
                                    &range_size);
                            u32 size = mapping_pointer->uv_maps.size();
                            mapping_pointer->uv_maps.resize(size + range_size);
                            for (u32 j = 0; j < range_size; j++)
                            {
                                std::array<u8, 3> local_var_arr;
                                local_offset = utils::convert_bytes(
                                        m_file_view, 
                                        local_offset, 
                                        local_var_arr.data(),
                                        local_var_arr.size()
                                        );
                                mapping_pointer->uv_maps[size + j].start = 0;
                                for (auto j = 0; j < 3; j++)
                                {
                                    mapping_pointer->uv_maps[size + j].start &= ((u32)local_var_arr[j] << (j));
                                }
                                local_offset = utils::convert_bytes(
                                        m_file_view, 
                                        local_offset, 
                                        &mapping_pointer->uv_maps[size+j].glyph_id
                                        );
                            }
                        }
                        else 
                        {
                            curr_var.is_default = true;
                            u64 local_offset = curr_var.default_offset;
                            local_offset = utils::convert_bytes(
                                    m_file_view, 
                                    local_offset, 
                                    &range_size);
                            u32 size = mapping_pointer->unicode_ranges.size();
                            curr_var.default_offset = size;
                            mapping_pointer->unicode_ranges.resize(size + range_size);
                            for (u32 j = 0; j < range_size; j++)
                            {
                                std::array<u8, 3> local_var_arr;
                                local_offset = utils::convert_bytes(
                                        m_file_view, 
                                        local_offset, 
                                        local_var_arr.data(),
                                        local_var_arr.size()
                                        );
                                mapping_pointer->unicode_ranges[size + j].start = 0;
                                for (auto j = 0; j < 3; j++)
                                {
                                    mapping_pointer->unicode_ranges[size + j].start &= ((u32)local_var_arr[j] << (j));
                                }
                                local_offset = utils::convert_bytes(
                                        m_file_view, 
                                        local_offset, 
                                        &mapping_pointer->unicode_ranges[size+j].count
                                        );
                            }
                        }
                    }
                }
                break;
            default: {}
        }
        m_subtables.insert({
                cmap_utils::get_index(platform_id, encoding_id, (u32)language),
                std::move(curr_subtable)});
    }
}}}
