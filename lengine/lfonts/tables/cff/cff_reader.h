#ifndef L_FONTS_CFF_READER_H
#define L_FONTS_CFF_READER_H
#include "cff_standard.h"
#include "tables/cff/cff_filewriter.h"
#include "tables/render_table.h"
#include "types.h"
#include "utils.h"
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <variant>
#include <vector>
namespace lina { namespace font { namespace tables{
    class cff : public glyph_table
    {
        public:
            using OperandTypes = std::variant<i8, u16, i16, i32, f64>;
            using CharStringOperandTypes = std::variant<i8, i16, f64>;
            struct cff_font_props
            {
                std::string name;
                std::string full_name;
                std::string family_name;
                std::string copy_right;
                std::string version;
                std::string weight;
                u64 private_offset = 0;
                i64 local_subr_offset = 0;
                u16 local_subr_bias = 0;
                u64 private_size = 0;
                u64 curr_width = 0;
                int encoding = 0;
                int charset = 0;
                int charstringtype = 2;
                int fd_select = 0;
                u64 charstring_offset = 0;
                std::vector<std::vector<u8>> subroutines;
                std::vector<std::vector<u8>> charstring_data;

                // private values
                u64 nominal_width = 0;
                u64 default_width = 0;
                f64 expansion_factor = 0.06;
                f64 left_offset = 0.;
                f64 top_offset = 0.;

                friend std::ostream& operator<<(std::ostream& os, cff_font_props f)
                {
                    os << "Name info: " << f.name 
                        << ", " << f.family_name  << ", " << f.full_name
                        << ", " << f.version << ", " << f.weight << ", " << f.copy_right << '\n';

                    return os;
                }
            };

            cff(const std::string_view& view, std::unique_ptr<cff_writer> writer);
            virtual void add_character_render(u64 character_index, u64 font_idx = 0) override;
            std::variant<i8, u16, i16, i32, f64> parse_dict_operand(const std::vector<u8>& ops, u64& idx);
            std::variant<i8, i16, f64> parse_charstring_operand(const std::vector<u8>& ops, u64& idx);
            std::vector<std::vector<u8>> read_index(const std::string_view& str, u64& offset);
            std::pair<u32, std::vector<u8>> read_encodings(const std::string_view& str, u64& offset);

            template <typename T, int ArrSize>
                std::vector<std::vector<u8>> read_index_data(
                        const std::string_view& str, 
                        u64& curr_offset, 
                        u16 count,
                        u64 data_offset);
            inline std::optional<const std::reference_wrapper<const std::string>> sid_to_str(u16 sid);

            void apply_dict_operation(g_cff_dict_ops op, u64 font_idx, std::vector<OperandTypes>& args);
            void apply_charstring_operation(g_cff_charstring_ops op, 
                    u64 font_idx, 
                    std::vector<CharStringOperandTypes>& args,
                    std::vector<CharStringOperandTypes>& transient_arr,
                    i64 offset = 0);
            void call_charstring_routine(
                    u64 font_idx,
                    std::vector<u8>& data,
                    std::vector<CharStringOperandTypes>& args,
                    std::vector<CharStringOperandTypes>& transient_arr
                    );

            inline virtual void save_current_glyphs(std::string path) override{
                m_writer->save_to_file(path);
            }
        public:
            u8 m_major_version;
            u8 m_minor_version;
            std::string_view m_file_view;
            std::vector<std::string> m_sid_extension;
            std::vector<cff_font_props> m_fonts;
            std::vector<std::vector<u8>> m_global_subrs;
            u16 m_global_subr_bias = 0;
            std::unique_ptr<cff_writer> m_writer;
    };
    inline std::optional<const std::reference_wrapper<const std::string>> cff::sid_to_str(u16 sid)
    {
        return utils::possibly_unsafe_get<std::string>(g_cff_sid, m_sid_extension, sid);
    }
    template <typename T, int ArrSize>
        std::vector<std::vector<u8>> cff::read_index_data(
                const std::string_view& str, 
                u64& curr_offset, 
                u16 count,
                u64 data_offset)
        {
            T local_start;
            T local_end;
            std::array<u8, ArrSize> local_start_arr;
            std::array<u8, ArrSize> local_end_arr;

            std::vector<std::vector<u8>> data;
            data.resize(count);
            curr_offset = utils::convert_bytes(
                    str, 
                    curr_offset,
                    local_start_arr.data(),
                    ArrSize
                    );
            local_start = utils::convert_big_endian<T, ArrSize>(local_start_arr);
            for (int i = 1; i < count + 1; i++)
            {
                curr_offset = utils::convert_bytes(
                        str, 
                        curr_offset,
                        local_end_arr.data(),
                        ArrSize
                        );
                local_end = utils::convert_big_endian<T, ArrSize>(local_end_arr);
                if (local_end == local_start) continue;

                data[i-1].resize(local_end - local_start);
                utils::convert_bytes(
                        str, 
                        data_offset + local_start - 1,
                        data[i-1].data(),
                        data[i-1].size()
                        );
                local_start = local_end;
            }

            curr_offset = data_offset + local_start - 1;
            return data;
        }
}}}
#endif
