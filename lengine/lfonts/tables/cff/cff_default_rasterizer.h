#ifndef L_FONT_CFF_DEFAULT_RENDERER_H
#define L_FONT_CFF_DEFAULT_RENDERER_H
#include "tables/cff/cff_filewriter.h"
#include "lmath/poly.h"
#include <atomic>
#include <barrier>
#include <functional>
#include <optional>
#include <utility>
extern "C"
{
    #include "laughic/graphics_util.h"
    #include "laughic/shapes.h"
}
namespace lina { namespace font { namespace tables { namespace defaults {
    class cff_rasterizer : public cff_writer
    {
        public: 
            using ivec2 = std::pair<f64, f64>;
            using vec2 = std::pair<f64, f64>;
            cff_rasterizer();
            virtual ~cff_rasterizer();
            virtual void move_cursor(f64 x, f64 y) override;
            virtual void line_to(f64 x, f64 y) override;
            virtual void curve_to(f64 x1, f64 y1, f64 x2, f64 y2, f64 x3, f64 y3) override;
            virtual void submit() override;
            virtual void reset() override;
            virtual void save_to_file(std::string path) override;
            
        public: // helpers
            void render();
            void render_region(int start_x, int end_x, int start_y, int end_y);
            std::vector<f64> solve_depressed_cubic(f64 a, f64 b);
            std::array<f64, 2> to_depressed(f64 a, f64 b, f64 c, f64 d);
            std::vector<f64> solve_cubic(f64 a, f64 b, f64 c, f64 d);
            std::pair<f64, i64> line_intersect(vec2 ray_origin, const std::array<vec2, 2>& points);
            std::pair<f64, i64> curve_intersect(vec2 ray_origin, u64 idx);
            vec2 curve_interpoloate(const std::array<vec2, 4>& points, f64 t);
            vec2 curve_grad(u64 idx, f64 t);
            vec2 curve_norm(u64 idx, f64 t);
            std::pair<f64, i64> sdf_curve(vec2 origin, const std::array<vec2, 4>& points);
            std::pair<f64, i64> sdf_line(vec2 origin, const std::array<vec2, 2>& points);
            std::pair<math::polynomial, math::polynomial> get_cubic_bezier_standard_polys(const std::array<vec2, 4>& points) const;
            std::pair<math::polynomial, math::polynomial> get_cubic_bezier_standard_polys(u64 idx) const;
            void render_sample(int i, int j, u16 sample_count, std::atomic<f64>& min_dist,
                    std::atomic<u16>& positive_count, std::atomic<u16>& negative_count, u16 iterations = 1);
        public:
            f64 m_x0;
            f64 m_y0;
            std::optional<f64> m_original_x = std::nullopt;
            std::optional<f64> m_original_y = std::nullopt;
            u64 sdf_pixel_width = 8;
            i64 m_winding_correction = 1;
            std::vector<std::array<vec2, 4>> m_curves;
            std::vector<std::array<vec2, 2>> m_lines;
            Canvas m_canvas;
            u64 glyph_counter = 0;
            u64 m_y_offset = 0;
            u16 m_chars_on_line = 0;
            u64 m_curr_advance_height = 0;
            std::vector<std::pair<math::polynomial, math::polynomial>> m_curve_polynomials_cache;
            std::vector<std::thread> m_threads;
            std::unique_ptr<std::barrier<>> m_barrier;
            std::atomic<bool> m_stop_threads;
            std::function<void(u16 thread_idx)> m_thread_work;
            u16 m_sample_count;
            std::vector<std::pair<u64, u64>> m_contour_begins = {{0, 0}};
            std::vector<i8> m_contour_winding;
            f64 m_winding_accumulator = 0.;
            u64 m_x_offset = 0;
            struct sdf_canvas
            {
                std::vector<f64> data;
                u64 width;
                u64 height;
            };
            sdf_canvas m_sdf_canvas;
    };
}}}}
#endif
