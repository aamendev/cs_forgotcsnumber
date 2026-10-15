#ifndef L_FONT_CFF_FILE_WRITER_H
#define L_FONT_CFF_FILE_WRITER_H
#include "types.h"
#include <iostream>
namespace lina { namespace font { namespace tables {
    class cff_writer
    {
        public:
        virtual ~cff_writer() = default;
        virtual void reset() = 0;
        virtual void move_cursor(f64 x, f64 y) = 0;
        virtual void line_to(f64 x, f64 y) = 0;
        virtual void curve_to(f64 x1, f64 y1, f64 x2, f64 y2, f64 x3, f64 y3) = 0;
        virtual void submit() = 0;
        virtual void save_to_file(std::string path) = 0;

        inline void push_width(f64 w);
        inline void reserve_space(u64 width_count);
        public:
            u64 m_total_offset = 0;
            u64 m_curr_offset = 0;
            u64 m_latest_min_x = 0;
            u64 m_latest_width = 0;
            u64 m_latest_min_y = 0;
            u64 m_latest_height = 0;
            f64 left_offset = 0.;
            f64 top_offset = 0.;
            std::array<std::array<f64, 3>, 2> m_matrix = 
            {(std::array<f64, 3>){1e-3, 0, 0}, {0, 1e-3, 0}};
    };
    inline void cff_writer::push_width(f64 w) 
    {
        m_total_offset += w;
    }
}}}
#endif
