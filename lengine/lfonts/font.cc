#include "font.h"
#include "lcpp_utils/lstring.h"
#include <filesystem>
#include <iostream>
#include <memory>
#include <string>
#include <string_view>
#include <variant>
#include <vector>
#include "tables/cff/cff_filewriter.h"
#include "tables/cff/cff_default_rasterizer.h"
#include "tables/cmap/cmap_reader.h"
#include "utils.h"
#include "tables/cff/cff_reader.h"
namespace lina { namespace font {
    void font_reader::open_file(std::string&& path)
    {
        auto otf_file = cpp_utils::string::load_file_to_string(std::move(path));
        u32 version_byte = 0;
        u16 num_tables = 0;
        utils::convert_bytes(otf_file, 0, &version_byte);
        utils::convert_bytes(otf_file, sizeof(version_byte), &num_tables);

        u64 curr_offset = sizeof(u32) + 4 * sizeof(u16);
        for (int i = 0; i < num_tables; i++)
        {
            std::array<u8, 4> first_table;
            curr_offset = utils::convert_bytes(otf_file, curr_offset, (u8*)&first_table[0], 4);
            curr_offset += sizeof(u32);

            u32 table_offset;
            curr_offset = utils::convert_bytes(otf_file, curr_offset, &table_offset);

            u32 table_length;
            curr_offset = utils::convert_bytes(otf_file, curr_offset, &table_length);

            std::string_view table_content(
                    otf_file.begin() + table_offset,
                    otf_file.begin() + table_offset + table_length);

            std::string str;
            for (auto c : first_table)
            {
                if (c != ' ')
                {
                    str.push_back(c);
                }
            }
            read_table(table_content, str);
        }
    }
    void font_reader::write_files(std::string directory, std::string name)
    {
        std::string settings;
        std::string settings_name = name + ".ldat";
        for (const auto& [code_point, glyph_data] : m_glyph_table->m_glyph_data_registry)
        {
            if (code_point == ' ') continue;
            settings = settings.append(
                    std::to_string(code_point) + ' ' + 
                    std::to_string(glyph_data.texture_bounds.start_col) + ' ' + 
                    std::to_string(glyph_data.texture_bounds.glyph_width) + ' ' + 
                    std::to_string(glyph_data.texture_bounds.start_row) + ' ' + 
                    std::to_string(glyph_data.texture_bounds.glyph_height) + '\n');
        }
        auto path = std::filesystem::path(directory) / settings_name;
        cpp_utils::string::write_string_to_file(std::move(settings),
                path);
        path = std::filesystem::path(directory) / std::string(name + ".lsdfs");
        m_glyph_table->save_current_glyphs(path);
    }
    void font_reader::read_table(const std::string_view& str, const std::string& table_name)
    {
        u64 curr_offset = 0;
        if (table_name == "CFF")
        {
            m_glyph_table = std::make_unique<tables::cff>(
                    str, 
                    std::make_unique<tables::defaults::cff_rasterizer>()
                    );
        }
        else if (table_name == "cmap")
        {
            m_cmap_table.init(str);
        }
    }
    std::pair<u64, u64> font_reader::read_files(std::string directory, std::string name, std::vector<f64>& data)
    {
        std::string settings_name = name + ".ldat";
        auto path = std::filesystem::path(directory) / settings_name;
        if (std::filesystem::exists(path))
        {
            data.clear();
            m_glyph_table->m_glyph_data_registry.clear();
            std::string loaded_str = cpp_utils::string::load_file_to_string(path);
            auto strs = cpp_utils::string::split(loaded_str, '\n');
            for (int i = 0; i < strs.size() - 1; i++)
            {
                auto new_strs = cpp_utils::string::split(strs[i], ' ');
                m_glyph_table->m_glyph_data_registry[std::stoul(new_strs[0])]
                    = {
                        .texture_bounds = {
                            .start_col = std::stoul(new_strs[1]),
                            .glyph_width = std::stoul(new_strs[2]),
                            .start_row = std::stoul(new_strs[3]),
                            .glyph_height = std::stoul(new_strs[4]),
                        }
                    };
            }
            auto new_strs = cpp_utils::string::split(strs.back(), ',');
            auto w = std::stoul(new_strs[0]);
            auto h = std::stoul(new_strs[1]);
            data.reserve(w * h);
            for (int i = 2; i < new_strs.size(); i++)
            {
                data.push_back(std::stof(new_strs[i]));
            }
            return {w, h};
        }
        return {0, 0};
    }
    void font_reader::add_characters(const std::vector<u64>& code_points)
    {
        for (const auto& cp : code_points)
        {
            u64 glyph_id = m_cmap_table.get_glyph_index(cp);
            if (glyph_id != 0)
            {
                m_glyph_table->add_character_render(glyph_id);
                if (cp != glyph_id)[[likely]]
                {
                    m_glyph_table->m_glyph_data_registry[cp] = 
                        m_glyph_table->m_glyph_data_registry[glyph_id];
                    m_glyph_table->m_glyph_data_registry.erase(glyph_id);
                }
            }
        }
    }
}}
