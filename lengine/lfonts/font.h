#ifndef L_FONT_H
#define L_FONT_H
#include <string>
#include <string_view>
#include <vector>
#include "tables/cmap/cmap_reader.h"
#include "tables/render_table.h"
#include "types.h"
namespace lina { namespace font {
    class font_reader
    {
        public:
            void open_file(std::string&& path);
            void read_table(const std::string_view& str, const std::string& table_name);
            void add_characters(const std::vector<u64>& code_points);
            void write_files(std::string directory, std::string name = "");
            std::pair<u64, u64> read_files(std::string directory, std::string name, std::vector<f64>& data);
        public:
            tables::cmap m_cmap_table;
            std::unique_ptr<tables::glyph_table> m_glyph_table;
    };
}}
#endif
