#include "cff_reader.h"
#include "cff_standard.h"
#include "lmath/utils.h"
#include "tables/cff/cff_utils.h"
#include "utils.h"
#include <cstddef>
#include <cstdlib>
#include <string>
#include <variant>
namespace lina { namespace font { namespace tables{
    cff::cff(const std::string_view& str, std::unique_ptr<cff_writer> writer): 
        m_file_view(str), m_writer(std::move(writer))
    {
        u64 curr_offset = 0;
        curr_offset = utils::convert_bytes(str, curr_offset, &m_major_version);
        curr_offset = utils::convert_bytes(str, curr_offset, &m_minor_version);

        u8 hdr_size;
        curr_offset = utils::convert_bytes(str, curr_offset, &hdr_size);

        curr_offset = hdr_size;
        auto names = cff::read_index(str, curr_offset);
        auto tddata = cff::read_index(str, curr_offset);
        auto stringdata = cff::read_index(str, curr_offset);
        { // global subrs
            u16 count;
            utils::convert_bytes(str, curr_offset, &count);
            m_global_subrs = cff::read_index(str, curr_offset);
            m_global_subr_bias = 107 * (count < 1240) + 1131 * (!(count < 1240) && (count < 33900))
                + 32768 * !(count < 33900);
        }
        for (const auto& v : stringdata) {
            m_sid_extension.emplace_back(v.cbegin(), v.cend());
        }
        {
            std::vector<OperandTypes> args;
            m_fonts.resize(names.size());
            for (int i = 0; i < names.size(); i++)
            {
                args.clear();
                m_fonts[i] = {.name{names[i].begin(), names[i].end()}};
                bool two_byte_command = false;
                u64 idx = 0;
                while (idx < tddata[i].size())
                {
                    if (tddata[i][idx] <= 21 || two_byte_command)
                    {
                        if (two_byte_command || tddata[i][idx] != 12)
                        {
                            g_cff_dict_ops op = (g_cff_dict_ops)tddata[i][idx];
                            if (two_byte_command)
                            {
                                op = (g_cff_dict_ops)((u16)op | ((u16)12<<8));
                            }
                            if (!args.empty())
                            {
                                apply_dict_operation(op, i, args);
                            }
                        }
                        two_byte_command = (tddata[i][idx] == 12 && !two_byte_command);
                        idx++;
                    }
                    else if (
                            (tddata[i][idx] >= 22 && tddata[i][idx] <=27) ||
                            (tddata[i][idx] == 31) ||
                            (tddata[i][idx] == 255)
                            )
                    {
                        idx++;
                    }
                    else 
                    {
                        args.push_back(cff::parse_dict_operand(tddata[i], idx));
                    }
                }
            }
        }
        {//private data + local_subrs
            std::vector<OperandTypes> args;
            for (int i = 0; i < m_fonts.size(); i++)
            {
                auto& f = m_fonts[i];
                std::vector<u8> private_data;
                private_data.resize(f.private_size);
                utils::convert_bytes(str, f.private_offset, private_data.data(), private_data.size());
                args.clear();
                bool two_byte_command = false;
                u64 idx = 0;
                while (idx < private_data.size())
                {
                    if (private_data[idx] <= 21 || two_byte_command)
                    {
                        if (two_byte_command || private_data[idx] != 12)
                        {
                            g_cff_dict_ops op = (g_cff_dict_ops)private_data[idx];
                            if (two_byte_command)
                            {
                                op = (g_cff_dict_ops)((u16)op | ((u16)12<<8));
                            }
                            if (!args.empty())
                            {
                                apply_dict_operation(op, i, args);
                            }
                        }
                        two_byte_command = (private_data[idx] == 12 && !two_byte_command);
                        idx++;
                    }
                    else if (
                            (private_data[idx] >= 22 && private_data[idx] <=27) ||
                            (private_data[idx] == 31) ||
                            (private_data[idx] == 255)
                            )
                    {
                        idx++;
                    }
                    else 
                    {
                        args.push_back(cff::parse_dict_operand(private_data, idx));
                    }
                }
                if (m_fonts[i].local_subr_offset != 0)
                {
                    u64 off =  m_fonts[i].local_subr_offset + (i64)m_fonts[i].private_offset;
                    u16 count;
                    utils::convert_bytes(str, off, &count);
                    m_fonts[i].subroutines = cff::read_index(str, off);
                    m_fonts[i].local_subr_bias = 
                        107 * (count < 1240) + 1131 * (!(count < 1240) && (count < 33900))
                        + 32768 * !(count < 33900);
                }
                u64 charstring_offset = f.charstring_offset;
                f.charstring_data = 
                    cff::read_index(m_file_view, charstring_offset);
            }
        }
    }

    void cff::add_character_render(u64 character_index, u64 font_idx)
    {
        if (font_idx >= m_fonts.size()) return;
        auto& f = m_fonts[font_idx];
        size_t sz = f.charstring_data.size();
        if (character_index >= sz) return;
        m_writer->top_offset = f.top_offset;
        m_writer->left_offset = f.left_offset;
        std::vector<CharStringOperandTypes> args;
        std::vector<CharStringOperandTypes> transient_arr;
        m_writer->reset();
        u64 curr_off = m_writer->m_total_offset;
        call_charstring_routine(
                font_idx, f.charstring_data[character_index], 
                args, transient_arr);
        if (curr_off == m_writer->m_total_offset)
        {
            m_writer->push_width(f.default_width);
        }
        m_writer->submit();
        m_glyph_data_registry[character_index].texture_bounds=
        {
            .start_col = m_writer->m_latest_min_x,
            .glyph_width = m_writer->m_latest_width,
            .start_row = m_writer->m_latest_min_y,
           .glyph_height =  m_writer->m_latest_height,
        };
    }
    void cff::call_charstring_routine(
                    u64 font_idx,
                    std::vector<u8>& data,
                    std::vector<CharStringOperandTypes>& args,
                    std::vector<CharStringOperandTypes>& transient_arr
                    )
    {
        bool two_byte_command = false;
        u64 idx = 0;
        while (idx < data.size())
        {
            if ((data[idx] <= 31 || two_byte_command) && data[idx] != 28)
            {
                if (two_byte_command || data[idx] != 12)
                {
                    g_cff_charstring_ops op = (g_cff_charstring_ops)data[idx];
                    if (two_byte_command)
                    {
                        op = (g_cff_charstring_ops)((u16)op | ((u16)12<<8));
                    }
                    if (!args.empty())
                    {
                        apply_charstring_operation(op, font_idx, args, transient_arr);
                    }
                }
                two_byte_command = (data[idx] == 12 && !two_byte_command);
                idx++;
            }
            else 
            {
                args.push_back(cff::parse_charstring_operand(data, idx));
            }
        }
    }
    std::vector<std::vector<u8>> cff::read_index(const std::string_view& str, u64& curr_offset)
    {
        u16 count;
        curr_offset = utils::convert_bytes(str, curr_offset, &count);
        if (count == 0) return {{}};

        u8 offset_byte_count;
        curr_offset = utils::convert_bytes(str, curr_offset, &offset_byte_count);

        u64 data_offset = curr_offset + offset_byte_count * (count+1);
        if (offset_byte_count == 1)
        {
            return read_index_data<u8, 1>(str, curr_offset, count, data_offset);
        }
        else if (offset_byte_count == 2)
        {
            return read_index_data<u16, 2>(str, curr_offset, count, data_offset);
        }
        else if (offset_byte_count == 3)
        {
            return read_index_data<u32, 3>(str, curr_offset, count, data_offset);
        }
        else
        {
            return read_index_data<u32, 4>(str, curr_offset, count, data_offset);
        }
    }

    std::variant<i8, u16, i16, i32, f64> cff::parse_dict_operand(const std::vector<u8>& ops, u64& idx)
    {
        if (ops[idx] >= 32 && ops[idx] <= 246)
        {
            i8 val = (i8)(ops[idx] - 139);
            idx++;
            return val;
        }
        else if (ops[idx] >= 247 && ops[idx] <= 250)
        {
            u16 val = (u16)(((u16)ops[idx] - 247) * 256  + (u16)ops[idx+1] + 108);
            idx += 2;
            return val;
        }
        else if (ops[idx] > 250)
        {
            i16 val = (i16)(-((i16)ops[idx] - 251) * 256  - (i16)ops[idx+1] - 108);
            idx+=2;
            return val;
        }
        else if (ops[idx] == 28)
        {
            i16 val = (i16)(((u16)ops[idx+1] << 8) | (u16)ops[idx+2]);
            idx += 3;
            return val;
        }
        else if (ops[idx] == 29)
        {
            i32 val = (i32)( 
                    ((u32)ops[idx+1] << 24) | 
                    ((u32)ops[idx+2] << 16) | 
                    ((u32)ops[idx+3] << 8) |
                    ((u32)ops[idx+4])
                    );
            idx += 5;
            return val;
        }
        else if (ops[idx] == 30)
        {
            std::string float_str;
            u64 shift = 1;
            bool is_second = false; 
            u8 curr_nip = (ops[idx+shift]>>(4*!is_second)) &0x0f;
            u64 sub = 0;
            while (curr_nip != 0xf)
            {
                if (curr_nip >= 0 && curr_nip <= 9)
                {
                    float_str.push_back(curr_nip + '0');
                }
                else if (curr_nip == 0xa)
                {
                    float_str.push_back('.');
                }
                else if (curr_nip == 0xb)
                {
                    float_str.push_back('e');
                }
                else if (curr_nip == 0xc)
                {
                    float_str.push_back('e');
                    float_str.push_back('-');
                    sub++;
                }
                else if (curr_nip == 0xe)
                {
                    float_str.push_back('-');
                }
                shift += is_second;
                is_second = !is_second;
                curr_nip = (ops[idx+shift]>>(4*!is_second)) &0x0f;
            }
            idx += shift + 1;
            return (f64)std::stod(float_str);
        }
        else 
        {
            return 0;
        }
    }
    std::variant<i8, i16, f64> cff::parse_charstring_operand(const std::vector<u8>& ops, u64& idx)
    {
        if (ops[idx]>= 32 && ops[idx] <= 246)
        {
            i8 val = (i8)(ops[idx] - 139);
            idx++;
            return val;
        }
        else if (ops[idx] >= 247 && ops[idx] <= 250)
        {
            i16 val = (i16)(ops[idx] - 247) * 256 + ops[idx+1] + 108;
            idx += 2;
            return val;
        }
        else if (ops[idx] >= 251 && ops[idx] <= 254)
        {
            i16 val = -(((i16)ops[idx] - 251) * 256) - (i16)ops[idx+1] - 108;
            idx += 2;
            return val;
        }
        else if (ops[idx] == 28)
        {
            i16 val = ((u16)ops[idx+1] << 8) + ops[idx+2];
            idx += 3;
            return val;
        }
        else
        {
            i16 init_val = ((u16)ops[idx+1] << 8) + (u16)ops[idx+2];
            f64 frac = (f64)(((u16)ops[idx+3] << 8) + ops[idx+4])/((u16)1 << 16);
            f64 val = (f64)init_val - (val < 0) * frac + !(val < 0) * frac;
            idx += 5;
            return val;
        }
    }
    std::pair<u32, std::vector<u8>> cff::read_encodings(const std::string_view& str, u64& curr_offset)
    {
        u8 format;
        curr_offset = utils::convert_bytes(str, curr_offset, &format);

        u8 count;
        curr_offset = utils::convert_bytes(str, curr_offset, &count);

        std::vector<u8> data;
        data.resize(count * (format == 0) + 2 * count * (format == 1));
        curr_offset = utils::convert_bytes(
                str, curr_offset, 
                &data[0],
                data.size());
        return {format, data};
    }
    void cff::apply_dict_operation(g_cff_dict_ops op, u64 font_idx, std::vector<OperandTypes>& args)
    {
        switch(op)
        {
            case g_cff_dict_ops::version:
                {
                    m_fonts[font_idx].version = 
                        sid_to_str(get_variant<u16>(args[0]))->get();
                }
                break;
            case g_cff_dict_ops::full_name:
                {
                    m_fonts[font_idx].full_name = sid_to_str(get_variant<u16>(args[0]))->get();
                }
                break;
            case g_cff_dict_ops::family_name:
                {
                    m_fonts[font_idx].family_name = sid_to_str(get_variant<u16>(args[0]))->get();
                }
                break;
            case g_cff_dict_ops::copyright:
                {
                    m_fonts[font_idx].copy_right = sid_to_str(get_variant<u16>(args[0]))->get();
                }
                break;
            case g_cff_dict_ops::weight:
                {
                    m_fonts[font_idx].weight = sid_to_str(get_variant<u16>(args[0]))->get();
                }
                break;
            case g_cff_dict_ops::char_strings:
                {
                    m_fonts[font_idx].charstring_offset = std::visit([](auto value) {
                            return static_cast<u64>(value);
                            }, args[0]);
                }
                break;
            case g_cff_dict_ops::nominal_width_x:
                {
                    m_fonts[font_idx].nominal_width = 
                        std::visit([](auto value) {
                                return static_cast<i64>(value);
                                }, args[0]);
                }
                break;
            case g_cff_dict_ops::font_matrix:
                {
                    for (int j = 0; j < 2; j++)
                    {
                        for (int i = 0; i < 3; i++)
                        {
                                m_writer->m_matrix[j][i] = get_variant<f64>(args[i * 2 + j]);
                        }
                    }
                }
                break;
            case g_cff_dict_ops::lprivate:
                {
                    m_fonts[font_idx].private_size = get_variant<u64>(args[0]);
                    m_fonts[font_idx].private_offset = get_variant<u64>(args[1]);
                }
                break;
            case g_cff_dict_ops::expansion_factor:
                {
                    m_fonts[font_idx].expansion_factor = get_variant<f64>(args[0]);
                }
                break;
            case g_cff_dict_ops::default_width_x:
                {
                    m_fonts[font_idx].default_width = get_variant<f64>(args[0]);
                }
                break;
            case g_cff_dict_ops::font_bbox:
                {
                    m_fonts[font_idx].left_offset = -get_variant<f64>(args[0]);
                    m_fonts[font_idx].top_offset = -get_variant<f64>(args[1]);
                }
                break;
            case g_cff_dict_ops::subrs:
                {
                    m_fonts[font_idx].local_subr_offset = get_variant<i64>(args[0]);
                }
                break;
            case g_cff_dict_ops::ros:
                {
                    std::cout << "CID Detected\n";
                }
                break;
            case g_cff_dict_ops::syntheic_base:
                {
                    std::cout << "Synthetic\n";
                }
                break;
            case g_cff_dict_ops::post_script:
                {
                    std::cout << "Postscript\n";
                }
                break;
            default:{}
        }
        args.clear();
    }
    void cff::apply_charstring_operation(
            g_cff_charstring_ops op, 
            u64 font_idx, 
            std::vector<CharStringOperandTypes>& args, 
            std::vector<CharStringOperandTypes>& transient_arr, 
            i64 offset)
    {
        auto& f = m_fonts[font_idx];
        i32 idx = 0;
        switch (op)
        {
            case g_cff_charstring_ops::rmoveto:
                {
                    auto dy = get_variant<f64>(args.back());
                    args.pop_back();
                    auto dx = get_variant<f64>(args.back());
                    args.pop_back();
                    if (!args.empty())[[unlikely]]
                    {
                        auto w = get_variant<f64>(args.back());
                        args.pop_back();
                        m_writer->push_width(f.nominal_width + w);
                        f.curr_width = f.nominal_width + w;
                    }
                    m_writer->move_cursor(dx, dy);
                }
                break;
            case g_cff_charstring_ops::endchar:
                {
                    if (!args.empty())
                    {
                        auto w = get_variant<f64>(args.back());
                        args.pop_back();
                        m_writer->push_width(f.nominal_width + w);
                        f.curr_width = f.nominal_width + w;
                    }
                    args.clear();
                }
                break;
            case g_cff_charstring_ops::hmoveto:
                {
                    auto dx = get_variant<f64>(args.back());
                    args.pop_back();
                    if (!args.empty())[[unlikely]]
                    {
                        auto w = get_variant<f64>(args.back());
                        args.pop_back();
                        m_writer->push_width(f.nominal_width + w);
                        f.curr_width = f.nominal_width + w;
                    }
                    m_writer->move_cursor(
                        dx,
                            0
                            );
                }
                break;
            case g_cff_charstring_ops::vmoveto:
                {
                    auto dy = get_variant<f64>(args.back());
                    args.pop_back();
                    if (!args.empty())[[unlikely]]
                    {
                        auto w = get_variant<f64>(args.back());
                        args.pop_back();
                        m_writer->push_width(f.nominal_width + w);
                        f.curr_width = f.nominal_width + w;
                    }
                    m_writer->move_cursor(
                        0,
                            dy
                            );
                }
                break;
            case g_cff_charstring_ops::rlineto:
                {
                    for (int i = 0; i < args.size(); i+=2)
                    {
                        m_writer->line_to(
                                get_variant<f64>(args[i]),
                                get_variant<f64>(args[i+1])
                            );
                    }
                    args.clear();
                }
                break;
            case g_cff_charstring_ops::rlinecurve:
                {
                    for (int i = 0; i < args.size()-6; i+=2)
                    {
                        m_writer->line_to(
                                get_variant<f64>(args[i]),
                                get_variant<f64>(args[i+1])
                            );
                    }
                    int i = args.size() - 6;
                        m_writer->curve_to(
                                get_variant<f64>(args[i]),
                                get_variant<f64>(args[i+1]),
                                get_variant<f64>(args[i+2]),
                                get_variant<f64>(args[i+3]),
                                get_variant<f64>(args[i+4]),
                                get_variant<f64>(args[i+5])
                             );
                    args.clear();
                }
                break;
            case g_cff_charstring_ops::rcurveline:
                {
                    for (int i = 0; i < args.size()-2; i+=6)
                    {
                        m_writer->curve_to(
                                get_variant<f64>(args[i]),
                                get_variant<f64>(args[i+1]),
                                get_variant<f64>(args[i+2]),
                                get_variant<f64>(args[i+3]),
                                get_variant<f64>(args[i+4]),
                                get_variant<f64>(args[i+5])
                             );
                    }
                    int i = args.size() - 2;
                    m_writer->line_to(
                            get_variant<f64>(args[i]),
                            get_variant<f64>(args[i+1])
                            );
                    args.clear();
                }
                break;
            case g_cff_charstring_ops::hlineto:
                {
                    u64 curr_line = 0;
                    for (int i = 0; i < args.size(); i++)
                    {
                        f64 val = get_variant<f64>(args[i]);
                        m_writer->line_to(
                                (curr_line%2 == 0) * val,
                                (curr_line%2 == 1) * val);
                        curr_line++;
                    }
                    args.clear();
                }
                break;
            case g_cff_charstring_ops::vlineto:
                {
                    u64 curr_line = 0;
                    for (int i = 0; i < args.size(); i++)
                    {
                        f64 val = get_variant<f64>(args[i]);
                        m_writer->line_to(
                                (curr_line%2 == 1) * val,
                                (curr_line%2 == 0) * val);
                        curr_line++;
                    }
                    args.clear();
                }
                break;
            case g_cff_charstring_ops::rrcurveto:
                {
                    for (int i = 0; i < args.size(); i+=6)
                    {
                        m_writer->curve_to(
                                get_variant<f64>(args[i]),
                                get_variant<f64>(args[i+1]),
                                get_variant<f64>(args[i+2]),
                                get_variant<f64>(args[i+3]),
                                get_variant<f64>(args[i+4]),
                                get_variant<f64>(args[i+5])
                             );
                    }
                    args.clear();
                }
                break;
            case g_cff_charstring_ops::hhcurveto:
                {
                    f64 offset_y = ((args.size()) %2 == 1) 
                        * get_variant<f64>(args[0]);
                    idx += ((args.size()) % 2 == 1);
                    for (int i = idx; i < args.size(); i+=4)
                    {
                        m_writer->curve_to(
                                get_variant<f64>(args[i]),
                                offset_y, 
                                get_variant<f64>(args[i+1]),
                                get_variant<f64>(args[i+2]),
                                get_variant<f64>(args[i+3]),
                                0
                             );
                    }
                    args.clear();
                }
                break;
            case g_cff_charstring_ops::vvcurveto:
                {
                    f64 offset_x = (args.size() %2 == 1) * get_variant<f64>(args[0]);
                    idx += (args.size() % 2 == 1);
                    for (int i = idx; i < args.size(); i+=4)
                    {
                        m_writer->curve_to(
                                offset_x, 
                                get_variant<f64>(args[i]),
                                get_variant<f64>(args[i+1]),
                                get_variant<f64>(args[i+2]),
                                0,
                                get_variant<f64>(args[i+3])
                             );
                    }
                    args.clear();
                }
                break;
            case g_cff_charstring_ops::vhcurveto:
                {
                    int curve_idx = 0;
                    int has_extra = (args.size() % 4) == 1;
                    int addition =  has_extra * get_variant<f64>(args[args.size() - 1]);
                    for (int i = 0; i < args.size() - has_extra; i+=4)
                    {
                        m_writer->curve_to(
                                get_variant<f64>(args[i]) * (curve_idx % 2 == 1), 
                                get_variant<f64>(args[i]) * (curve_idx % 2 == 0), 
                                get_variant<f64>(args[i+1]),
                                get_variant<f64>(args[i+2]),
                                get_variant<f64>(args[i+3]) * (curve_idx % 2 == 0) + 
                                addition * (i == args.size() - 5) * (curve_idx % 2 == 1),
                                get_variant<f64>(args[i+3]) * (curve_idx % 2 == 1) + 
                                addition * (i == args.size() - 5) * (curve_idx % 2 == 0)
                             );
                        curve_idx++;
                    }
                    args.clear();
                }
                break;
            case g_cff_charstring_ops::hvcurveto:
                {
                    int curve_idx = 0;
                    int has_extra = (args.size() % 4) == 1;
                    int addition =  has_extra * get_variant<f64>(args[args.size() - 1]);
                    for (int i = 0; i < args.size() - has_extra; i+=4)
                    {
                        m_writer->curve_to(
                                get_variant<f64>(args[i]) * (curve_idx % 2 == 0), 
                                get_variant<f64>(args[i]) * (curve_idx % 2 == 1), 
                                get_variant<f64>(args[i+1]),
                                get_variant<f64>(args[i+2]),
                                get_variant<f64>(args[i+3]) * (curve_idx % 2 == 1) +
                                addition * (i == args.size() - 5) * (curve_idx % 2 == 0),
                                get_variant<f64>(args[i+3]) * (curve_idx % 2 == 0) +
                                addition * (i == args.size() - 5) * (curve_idx % 2 == 1)
                             );
                        curve_idx++;
                    }
                    args.clear();
                }
                break;
            case g_cff_charstring_ops::add:
                {
                    auto v2 = get_variant<f64>(args.back());
                    args.pop_back();
                    auto v1 = get_variant<f64>(args.back());
                    args.pop_back();
                    args.push_back(v1 + v2);
                }
                break;
            case g_cff_charstring_ops::abs:
                {
                    auto v1 = get_variant<f64>(args.back());
                    args.pop_back();
                    args.push_back(std::abs(v1));
                }
                break;
            case g_cff_charstring_ops::neg:
                {
                    auto v1 = get_variant<f64>(args.back());
                    args.pop_back();
                    args.push_back(-v1);
                }
                break;
            case g_cff_charstring_ops::sqrt:
                {
                    auto v1 = get_variant<f64>(args.back());
                    args.pop_back();
                    args.push_back(std::sqrt(v1));
                }
                break;
            case g_cff_charstring_ops::sub:
                {
                    auto v2 = get_variant<f64>(args.back());
                    args.pop_back();
                    auto v1 = get_variant<f64>(args.back());
                    args.pop_back();
                    args.push_back(v2 - v1);
                }
                break;
            case g_cff_charstring_ops::mul:
                {
                    auto v2 = get_variant<f64>(args.back());
                    args.pop_back();
                    auto v1 = get_variant<f64>(args.back());
                    args.pop_back();
                    args.push_back(v2 * v1);
                }
                break;
            case g_cff_charstring_ops::div:
                {
                    auto v2 = get_variant<f64>(args.back());
                    args.pop_back();
                    auto v1 = get_variant<f64>(args.back());
                    args.pop_back();
                    args.push_back(v2 / v1);
                }
                break;
            case g_cff_charstring_ops::land:
                {
                    auto v2 = get_variant<f64>(args.back());
                    args.pop_back();
                    auto v1 = get_variant<f64>(args.back());
                    args.pop_back();
                    args.push_back(1. * (v1 > 0 && v2 > 0));
                }
                break;
            case g_cff_charstring_ops::lor:
                {
                    auto v2 = get_variant<f64>(args.back());
                    args.pop_back();
                    auto v1 = get_variant<f64>(args.back());
                    args.pop_back();
                    args.push_back(1. * (v1 > 0 || v2 > 0));
                }
                break;
            case g_cff_charstring_ops::eq:
                {
                    auto v2 = get_variant<f64>(args.back());
                    args.pop_back();
                    auto v1 = get_variant<f64>(args.back());
                    args.pop_back();
                    args.push_back(1. * (math::utils::f64_eq(v1, v2)));
                }
                break;
            case g_cff_charstring_ops::ifelse:
                {
                    auto v2 = get_variant<f64>(args.back());
                    args.pop_back();
                    auto v1 = get_variant<f64>(args.back());
                    args.pop_back();

                    auto s2 = (args.back());
                    args.pop_back();
                    auto s1 = (args.back());
                    args.pop_back();
                    if (math::utils::f64_cmp(v1, v2) <= 0)
                    {
                        args.push_back(s1);
                    }
                    else 
                    {
                        args.push_back(s2);
                    }

                }
                break;
            case g_cff_charstring_ops::random:
                {
                    args.push_back((f64)std::rand()/RAND_MAX);
                }
                break;
            case g_cff_charstring_ops::drop:
                {
                    args.pop_back();
                }
                break;
            case g_cff_charstring_ops::dup:
                {
                    args.push_back(args.back());
                }
                break;
            case g_cff_charstring_ops::exch:
                {
                    auto v1 = get_variant<f64>(args.back());
                    args.pop_back();
                    auto v2 = get_variant<f64>(args.back());
                    args.pop_back();
                    args.push_back(v1);
                    args.push_back(v2);
                }
                break;
            case g_cff_charstring_ops::put:
                {
                    auto v1 = args.back();
                    args.pop_back();
                    transient_arr.push_back(v1);
                }
                break;
            case g_cff_charstring_ops::get:
                {
                    auto v1 = transient_arr.back();
                    transient_arr.pop_back();
                    args.push_back(v1);
                }
                break;
            case g_cff_charstring_ops::lreturn:
                {
                }
                break;
            case g_cff_charstring_ops::callsubr:
                {
                    auto off = get_variant<i64>(args.back());
                    args.pop_back();
                    call_charstring_routine(font_idx, 
                            m_fonts[font_idx].subroutines[off + m_fonts[font_idx].local_subr_bias], 
                            args, transient_arr); 
                }
                break;
            case g_cff_charstring_ops::callgsubr:
                {
                    auto off = get_variant<i64>(args.back());
                    args.pop_back();
                    call_charstring_routine(font_idx, 
                            m_global_subrs[off + m_global_subr_bias], 
                            args, transient_arr); 
                }
                break;
            default: 
                {
                    std::cout << "unsuported: " <<std::hex <<  (u16)op << '\n' << std::dec;
                }
        }
#if 0
        std::cout << "calling op " << std::hex << (u16)op << " with args: \n";
        for (const auto& arg : args)
        {
            std::visit([](auto v){std::cout << (f64)v;}, arg);
            std::cout << ", ";
        }
        args.clear();
        std::cout << '\n';
#endif
        //args.clear();
    }
    
}}}
