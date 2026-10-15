#ifndef L_FONTS_CMAP_SUBTABLE_TYPES_H
#define L_FONTS_CMAP_SUBTABLE_TYPES_H
#include <lmath/types.h>
#include <array>
#include <vector>
namespace lina { namespace font { namespace tables{
    namespace cmap_subs 
    {
        struct subtable_comparator
        {
            bool operator()(
                    const std::pair<u64, u8> v1,
                    const std::pair<u64, u8> v2) 
                const { return v1.second < v2.second;}
        };
        struct format 
        {
            virtual u64 get_glyph_index(u64 input_code) = 0;
            virtual u32 get_format_number() const = 0;
            virtual ~format() = default;
        };
        struct format0 : public format
        {
            inline virtual u32 get_format_number() const override {return 0;};
            std::array<u8, 256> glyph_ids;
            u64 get_glyph_index(u64 input_code) override;
        };
        struct format2 :  public format
        {
            inline virtual u32 get_format_number() const override {return 2;};
            struct subheader_record
            {
                u16 first_code;
                u16 entry_count;
                i16 id_delta;
                u16 id_range_offset;
            };
            std::array<u16, 256> subheader_keys;
            std::vector<subheader_record> records;
            std::vector<u16> glyph_ids;
            u64 get_glyph_index(u64 input_code) override;
        };
        struct format4 :  public format
        {
            inline virtual u32 get_format_number() const override {return 4;};
            std::vector<u16> end_code;
            std::vector<u16> start_code;
            std::vector<i16> id_delta;
            std::vector<u16> id_range_offset;
            std::vector<u16> glyph_ids;
            u64 get_glyph_index(u64 input_code) override;
        };
        struct format6 :  public format
        {
            inline virtual u32 get_format_number() const override {return 6;};
            u16 start_code;
            std::vector<u16> glyph_ids;
            u64 get_glyph_index(u64 input_code) override;
        };
        struct format8 :  public format
        {
            inline virtual u32 get_format_number() const override {return 8;};
            struct const_map_group
            {
                u32 start;
                u32 end;
                u32 glyph_id;
            };
            std::vector<const_map_group> groups;
            u64 get_glyph_index(u64 input_code) override;
        };
        struct format10 :  public format
        {
            inline virtual u32 get_format_number() const override {return 10;};
            u32 start_code;
            std::vector<u16> glyph_id;
            u64 get_glyph_index(u64 input_code) override;
        };
        struct format12 :  public format
        {
            inline virtual u32 get_format_number() const override {return 12;};
            struct const_map_group
            {
                u32 start;
                u32 end;
                u32 glyph_start;
            };
            std::vector<const_map_group> groups;
            u64 get_glyph_index(u64 input_code) override;
        };
        struct format13 :  public format
        {
            inline virtual u32 get_format_number() const override {return 13;};
            struct const_map_group
            {
                u32 start;
                u32 end;
                u32 glyph_id;
            };
            std::vector<const_map_group> groups;
            u64 get_glyph_index(u64 input_code) override;
        };
        struct format14 :  public format
        {
            inline virtual u32 get_format_number() const override {return 14;};
            struct var_selector
            {
                u32 selector;
                u32 default_offset;
                u32 nondefault_offset;
                bool is_default;
            };
            struct unicode_range
            {
                u32 start;
                u8 count;
            };
            struct uv_map
            {
                u32 start;
                u16 glyph_id;
            };
            std::vector<unicode_range> unicode_ranges;
            std::vector<uv_map> uv_maps;
            std::vector<var_selector> selectors;
            u64 get_glyph_index(u64 input_code) override;
        };
    }
}}}
#endif
