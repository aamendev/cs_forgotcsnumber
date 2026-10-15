#include "cff_default_rasterizer.h"
#include "lcpp_utils/lstring.h"
#include "lmath/poly.h"
#include "lmath/utils.h"
#include "lmath/vector2d.h"
#include "tables/cff/cff_standard.h"
#include <algorithm>
#include <atomic>
#include <cfloat>
#include <climits>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <memory>
#include <optional>
#include <random>
#include <sstream>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#define DEFAULT_RAST_EPS (1e-8)
namespace lina { namespace font { namespace tables { namespace defaults {
    cff_rasterizer::cff_rasterizer() : m_sample_count(25)
    {
        sdf_pixel_width = 16;
        u64 w = 1024;
        u64 h = w;
        m_sdf_canvas.data.resize(w * h);
        m_sdf_canvas.width = w;
        m_sdf_canvas.height = h;

        m_canvas.width = w;
        m_canvas.height = h;
        m_canvas.pixels = (u32*)malloc(w * h * sizeof(u32));
        for (int i= 0; i < w * h; i++)
        {
            m_canvas.pixels[i] = 0xff000000;
        }
        //fill(&m_canvas, 0xff000000);
        reset();
        m_curr_offset = 0;
        m_y_offset = sdf_pixel_width;
        const auto max_threads = 
            std::max(
                    (u32)1, 
                    std::min((u32)m_sample_count, 
                        std::thread::hardware_concurrency()));
        m_threads.reserve(max_threads);
        m_barrier = std::make_unique<std::barrier<>>(max_threads + 1);
        m_stop_threads = false;

        for (u16 curr_thread = 0; curr_thread < max_threads; curr_thread++)
        {
            m_threads.emplace_back([&, curr_thread]()
                    {
                    while (true)
                    {
                    m_barrier->arrive_and_wait();
                    if (m_stop_threads)
                    {
                    break;
                    }
                    m_thread_work(curr_thread);
                    m_barrier->arrive_and_wait();
                    }
                    });
        }
    }
    cff_rasterizer::~cff_rasterizer()
    {
        m_stop_threads = true;
        m_barrier->arrive_and_wait();
        for (auto& th : m_threads)
        {
            th.join();
        }
        m_threads.clear();
    }

    void cff_rasterizer::reset()
    {
        m_lines.clear();
        m_curves.clear();
        m_curve_polynomials_cache.clear();
        m_x0 = 0;
        m_y0 = 0;
        m_original_x = std::nullopt;
        m_original_y = std::nullopt;
        m_winding_accumulator = 0.;
        m_contour_winding.clear();
        m_contour_begins = {{0, 0}};
    }
    void cff_rasterizer::move_cursor(f64 x, f64 y) 
    {
        // y*=-1;
        if (m_original_x.has_value() && 
                (
                 !math::utils::f64_eq(m_x0, m_original_x.value())||
                 !math::utils::f64_eq(m_y0, m_original_y.value())))

        {
            m_lines.push_back(
                    {
                    std::make_pair(m_x0, m_y0), 
                    std::make_pair(m_original_x.value(), m_original_y.value()),
                    });
        }
        if (!m_lines.empty() || !m_curves.empty())
        {
            m_contour_begins.emplace_back(m_lines.size(), m_curves.size());
            m_contour_winding.push_back((m_winding_accumulator >= 0.) - !(m_winding_accumulator >= 0));
            m_winding_accumulator = 0.;
        }
        m_original_x = m_x0 + x;
        m_original_y = m_y0 + y;
        m_x0 += x;
        m_y0 += y;
    }
    void cff_rasterizer::line_to(f64 x, f64 y) 
    {
        //y*=-1;
        m_lines.push_back(
                {
                std::make_pair(m_x0, m_y0), 
                std::make_pair(m_x0 + x, m_y0 + y),
                });

        m_winding_accumulator += 0.5 * (m_x0 * y - m_y0 * x);

        m_x0 += x;
        m_y0 += y;
    }
    void cff_rasterizer::curve_to(f64 x1, f64 y1, f64 x2, f64 y2, f64 x3, f64 y3) 
    {
        //y1*=-1;
        //y2*=-1;
        //y3*=-1;
        m_winding_accumulator += 
            1./20. * 
            (
             6. * (m_x0 * y1 - m_y0 * x1) + 
             3. * (m_x0 * (y1 + y2) - m_y0 * (x1 + x2)) + 
             1. * (m_x0 * (y1 + y2 + y3) - m_y0 * (x1 + x2 + x3)) + 
             3. * ((m_x0 + x1) * (m_y0 + y1 + y2) - (m_y0 + y1) * (m_x0 + x1 + x2)) + 
             3. * ((m_x0 + x1) * (m_y0 + y1 + y2 + y3) - (m_y0 + y1) * (m_x0 + x1 + x2 + x3)) + 
             6. * ((m_x0 + x1 + x2) * (m_y0 + y1 + y2 + y3) - (m_y0 + y1) * (m_x0 + x1 + x2 + x3))
            );
        m_curves.push_back(
                {
                std::make_pair(m_x0, m_y0), 
                std::make_pair(m_x0 += x1, m_y0 += y1),
                std::make_pair(m_x0 += x2, m_y0 += y2),
                std::make_pair(m_x0 += x3, m_y0 += y3),
                });
    }
    void cff_rasterizer::render_sample(int i, int j, u16 sample_count,
            std::atomic<f64>& min_dist, 
            std::atomic<u16>& positive_count, std::atomic<u16>& negative_count, 
            u16 iterations)
    {
        f64 local_dist = 0.;
        u16 local_pos = 0;
        u16 local_neg = 0;
        constexpr f64 r = 0.5;
        thread_local std::mt19937_64 rng(
                std::random_device{}()
                );

        std::uniform_real_distribution<f64> dist(0., 1.0);

        for (u16 iter = 0; iter < iterations; iter++)
        {
            f64 u1 = dist(rng);
            f64 u2 = dist(rng);
            vec2 origin = {i + 0.5, j + 0.5};

            origin.first += r * std::sqrt(u2) * std::cos(2 * M_PI * u1);
            origin.second += r * std::sqrt(u2) * std::sin(2 * M_PI * u1);


            i8 sgn = 1;
            i8 curr_closest_line = -1;
            i8 curr_closest_curve = -1;
            f64 local_min_dist = FLT_MAX;
            for (int contidx = 0; contidx+ 1 < m_contour_begins.size(); contidx++)
            {
                u64 cont = m_contour_begins[contidx].first;
                u64 cont_end = m_contour_begins[contidx+1].first;
                for (int k = cont; k < cont_end; k++)
                {
                    const auto& l = m_lines[k];
                    auto [d, lsgn] = sdf_line(origin, l);

                    if (math::utils::f64_eq(d, local_min_dist))
                    {
                        if (m_contour_winding[contidx] == 1)
                        {
                            sgn = std::max(sgn, (i8)lsgn);
                        }
                        else 
                        {
                            sgn = std::min(sgn, (i8)lsgn);
                        }
                    }
                    else
                    {
                        sgn = lsgn * (d <= local_min_dist) + sgn * !(d <= local_min_dist);
                    }
                    local_min_dist = std::min(d, local_min_dist);
                }
            }
            for (int contidx = 0; contidx + 1 < m_contour_begins.size(); contidx++)
            {
                u64 cont = m_contour_begins[contidx].second;
                u64 cont_end = m_contour_begins[contidx+1].second;
                for (int k = cont; k < cont_end; k++)
                {
                    const auto& c = m_curves[k];
                    auto [d, lsgn] = sdf_curve(origin, c);
                    if (math::utils::f64_eq(d, local_min_dist))
                    {
                        if (m_contour_winding[contidx] == 1)
                        {
                            sgn = std::max(sgn, (i8)lsgn);
                        }
                        else 
                        {
                            sgn = std::min(sgn, (i8)lsgn);
                        }
                    }
                    else
                    {
                        sgn = lsgn * (d <= local_min_dist) + sgn * !(d <= local_min_dist);
                    }
                    local_min_dist = std::min(d, local_min_dist);
                }
            }
            local_dist += local_min_dist/sample_count;
            local_pos += (sgn == 1);
            local_neg += !(sgn == 1);
        }
        positive_count += local_pos;
        negative_count += local_neg;
        min_dist += local_dist;
    }
    void cff_rasterizer::render_region(int start_x, int end_x, int start_y, int end_y)
    {
        f64 sz = 1;
        f64 min_dist = 0.;
        std::atomic<f64> curr_dist = 0.;
        std::atomic<u16> positive_count = 0;
        std::atomic<u16> negative_count = 0;
        u16 samples_per_thread = std::floor(((f64)m_sample_count / m_threads.size()));
        for (i64 j = start_y; j < end_y; j++)
        {
            for (i64 i = start_x; i < end_x; i++)
            {
                i64 transformed_j = m_sdf_canvas.height -j - 1;
                curr_dist = 0;
                positive_count = 0;
                negative_count = 0;
                m_thread_work = [&](u16 thread_idx)
                {
                    u16 iterations = samples_per_thread + 
                        (thread_idx < m_sample_count % m_threads.size());
                    render_sample(
                            i, j, m_sample_count, curr_dist, 
                            positive_count,
                            negative_count,
                            iterations);
                };
                m_barrier->arrive_and_wait();
                m_barrier->arrive_and_wait();
                min_dist = curr_dist * m_winding_correction * 
                    ((negative_count < positive_count) + -!(negative_count < positive_count));
                    m_sdf_canvas.data[transformed_j * m_sdf_canvas.width + i]  = min_dist;
            }
        }
    }
    void cff_rasterizer::render()
    {
        if (m_curves.empty() && m_lines.empty()) return;
        if (
                !math::utils::f64_eq(m_x0, m_original_x.value())||
                !math::utils::f64_eq(m_y0, m_original_y.value()))

        {
            m_lines.push_back(
                    {
                    std::make_pair(m_x0, m_y0), 
                    std::make_pair(m_original_x.value(), m_original_y.value()),
                    });
        }
        if (!math::utils::f64_eq(m_winding_accumulator, 0.))
        {
            m_contour_begins.emplace_back(m_lines.size(), m_curves.size());
            m_contour_winding.push_back((m_winding_accumulator >= 0.) - !(m_winding_accumulator >= 0));
            m_winding_accumulator = 0.;
        }
        std::vector<std::pair<i64, i64>> to_shade;
        f64 scale = 64;
        f64 min_y = FLT_MAX;
        f64 max_y = -FLT_MAX;

        i64 winding_correction_candidates = 5;

        f64 transformed_current = (m_curr_offset) * m_matrix[0][0];
        transformed_current *= scale;
        transformed_current += ((m_chars_on_line + 1) * 2 - 1) * sdf_pixel_width;

        f64 transformed_total = (m_total_offset - m_curr_offset) * m_matrix[0][0];
        transformed_total *= scale;
        transformed_total += transformed_current;

        f64 transformed_top = top_offset * m_matrix[1][1];
        transformed_top *= scale;

        if (transformed_total + sdf_pixel_width >= m_canvas.width)
        {
            m_y_offset += m_curr_advance_height + 2 * sdf_pixel_width;
            m_total_offset = m_total_offset - m_curr_offset;
            m_curr_offset = 0;
            m_chars_on_line = 0;
            m_curr_advance_height = 0;

            transformed_current = (m_curr_offset) * m_matrix[0][0] + m_matrix[0][2];
            transformed_current *= scale;
            transformed_current += ((m_chars_on_line + 1) * 2 - 1)  * sdf_pixel_width;

            transformed_total = (m_total_offset - m_curr_offset) * m_matrix[0][0] + m_matrix[0][2];
            transformed_total *= scale;
            transformed_total += transformed_current;
        }
        for (auto& l : m_lines)
        {
            for (auto& p : l)
            {
                f64 x = p.first * m_matrix[0][0] + p.second * m_matrix[0][1] + m_matrix[0][2];
                f64 y = p.first * m_matrix[1][0] + p.second * m_matrix[1][1] + m_matrix[1][2];
                p.first = x * scale;
                p.second = y * scale;

                p.first += transformed_current;
                p.second += m_y_offset + transformed_top;

                min_y = std::min(min_y, p.second);
                max_y = std::max(max_y, p.second);
            }
        }
        m_curve_polynomials_cache.reserve(m_curves.size());
        for (auto& c : m_curves)
        {
            for (auto& p : c)
            {
                f64 x = p.first * m_matrix[0][0] + p.second * m_matrix[0][1] + m_matrix[0][2];
                f64 y = p.first * m_matrix[1][0] + p.second * m_matrix[1][1] + m_matrix[1][2];
                p.first = x * scale;
                p.second = y * scale;
                p.first += transformed_current;
                p.second += m_y_offset + transformed_top;
            }
            auto [xpoly, ypoly] = get_cubic_bezier_standard_polys(c);

            auto ysol = ypoly.derivative().laguerre();

            for (const auto yt : ysol)
            {
                if (yt < 0. || yt > 1.) continue;
                auto y = ypoly.evaluate(yt);
                min_y = std::min(min_y, y);
                max_y = std::max(max_y, y);
            }
            min_y = std::min(min_y, c[0].second);
            min_y = std::min(min_y, c[3].second);

            max_y = std::max(max_y, c[0].second);
            max_y = std::max(max_y, c[3].second);
            m_curve_polynomials_cache.emplace_back(xpoly, ypoly);
        }

        m_curr_advance_height = std::max(m_curr_advance_height, (u64)std::ceil(max_y - min_y));
        auto min_x = transformed_current;
        std::array<u64, 2> winding_counts {0, 0};
        for (int i = 0; i < winding_correction_candidates; i++)
        {
            f64 t = (f64)i/winding_correction_candidates;
            vec2 origin = {min_x, (1.-t) * min_y + t * max_y};
            f64 min_dist = FLT_MAX;
            for (const auto& l : m_lines)
            {
                auto [d, s] = line_intersect(origin, l);
                m_winding_correction = m_winding_correction * !(d < min_dist) 
                    + s * (d < min_dist);
                min_dist = std::min(d, min_dist);
            }
            u64 idx = 0;
            for (const auto& c : m_curves)
            {
                auto [d, s] = curve_intersect(origin, idx++);
                m_winding_correction = m_winding_correction * !(d < min_dist) 
                    + s * (d < min_dist);
                min_dist = std::min(d, min_dist);
            }
            winding_counts[
                (m_winding_correction > 0)
            ]++;
        }
        m_winding_correction = 
            (winding_counts[1] < winding_counts[0]) - !(winding_counts[1] < winding_counts[0]);

        m_latest_height = std::ceil(
                std::ceil(max_y) - std::floor(min_y) + 2 * sdf_pixel_width);
        m_latest_min_y = std::floor(min_y - sdf_pixel_width);
        m_latest_min_x = std::floor(transformed_current - sdf_pixel_width);
        m_latest_width = std::ceil(
                std::ceil(transformed_total) - std::floor(transformed_current) 
                + 2 * sdf_pixel_width);
        render_region(
                std::floor(transformed_current) - sdf_pixel_width, 
                std::ceil(transformed_total) + sdf_pixel_width, 
                std::floor(min_y) - sdf_pixel_width, 
                std::ceil(max_y) + sdf_pixel_width
                );

        m_curr_offset = m_total_offset;
    }
    void cff_rasterizer::submit()
    {
        render();
        glyph_counter++;
        m_chars_on_line++;
    }
    void cff_rasterizer::save_to_file(std::string path)
    {
        //save_to_img(&m_canvas, PNG, path.data());
        std::string file_string;
        std::stringstream ss(file_string);
        ss << m_sdf_canvas.width << ',' << m_sdf_canvas.height << ',';
        for (int i = 0; i < m_sdf_canvas.width * m_sdf_canvas.height - 1; i++)
        {
            ss << m_sdf_canvas.data[i] << ',';
        }
        ss << m_sdf_canvas.data.back();
        cpp_utils::string::write_string_to_file(ss.str(), std::move(path));
    }
    std::pair<f64, i64> cff_rasterizer::line_intersect(vec2 ray_origin, const std::array<vec2, 2>& points)
    {
        if (ray_origin.second < std::min(points[0].second, points[1].second)
                || ray_origin.second > std::max(points[0].second, points[1].second)
                || ray_origin.first > std::max(points[0].first, points[1].first))
        {
            return {std::numeric_limits<f64>().max(), 0};
        }
        vec2 d2 = {(points[1].first - points[0].first), points[1].second - points[0].second};
        f64 norm = std::sqrt(d2.first * d2.first + d2.second * d2.second);
        d2.second /= norm;
        d2.first /= norm;
        f64 cr = d2.second;
        if (std::abs(cr) < DEFAULT_RAST_EPS)
        {
            return {std::numeric_limits<f64>().max(), 0};
        }
        vec2 ba = {points[0].first - ray_origin.first, points[0].second - ray_origin.second};
        f64 t = (ba.first * d2.second - ba.second * d2.first) / cr;
        if (t < 0)
        {
            return {std::numeric_limits<f64>().max(), 0};
        }
        i64 winding = (points[0].second < points[1].second) - (points[1].second < points[0].second);
        return {ray_origin.first + t, winding};
    }
    std::pair<math::polynomial, math::polynomial> cff_rasterizer::get_cubic_bezier_standard_polys(const std::array<vec2, 4>& points) const
    {
        math::polynomial y_poly;
        math::polynomial x_poly;
        x_poly.m_terms.reserve(4);
        y_poly.m_terms.reserve(4);

        x_poly.m_terms.push_back({points[0].first ,0});
        x_poly.m_terms.push_back({-3*points[0].first +3 * points[1].first,1});
        x_poly.m_terms.push_back({3*points[0].first -6 * points[1].first + 3 * points[2].first,2});
        x_poly.m_terms.push_back({-points[0].first +3 * points[1].first - 3 * points[2].first + points[3].first,3});

        y_poly.m_terms.push_back({points[0].second ,0});
        y_poly.m_terms.push_back({-3*points[0].second +3 * points[1].second,1});
        y_poly.m_terms.push_back({3*points[0].second -6 * points[1].second + 3 * points[2].second,2});
        y_poly.m_terms.push_back({-points[0].second +3 * points[1].second - 3 * points[2].second + points[3].second,3});
        x_poly.clean_zero();
        y_poly.clean_zero();
        x_poly.sort();
        y_poly.sort();
        return 
        {
            x_poly, y_poly
        };
    }
    std::pair<f64, i64> cff_rasterizer::sdf_line(vec2 p, const std::array<vec2, 2>& points)
    {
        f64 delta = 0.;
        const auto& a = points[0];
        const auto& b = points[1];
        vec2 pa = {p.first-a.first, p.second - a.second};
        vec2 ba = {b.first-a.first, b.second - a.second};
        f64 h = std::clamp((pa.first * ba.first + pa.second * ba.second)/(ba.first * ba.first + ba.second * ba.second), 0.0, 1.0);
        h += 
            delta * math::utils::f64_eq(h, 0.)- 
            delta * math::utils::f64_eq(h, 1.);
        vec2 min_vec = {pa.first - ba.first * h, pa.second - ba.second * h};
        i64 sgn = 
            ((ba.second * min_vec.first + -ba.first * min_vec.second) >= 0) * 2 - 1;
        return {std::sqrt(min_vec.first * min_vec.first + min_vec.second * min_vec.second), sgn};
    }
    std::pair<f64, i64> cff_rasterizer::sdf_curve(vec2 origin, const std::array<vec2, 4>& points)
    {
        f64 delta = 0.;
        auto [poly1, poly2] = get_cubic_bezier_standard_polys(points);

        auto dpoly1 = poly1.derivative();
        auto dpoly2 = poly2.derivative();

        poly1.m_terms[0].coefficient -= origin.first;
        poly2.m_terms[0].coefficient -= origin.second;
        auto final_poly = (poly1 * dpoly1 + poly2 * dpoly2) ;
        auto sol = final_poly.laguerre();
        poly1.m_terms[0].coefficient += origin.first;
        poly2.m_terms[0].coefficient += origin.second;

        f64 min_dist = FLT_MAX;
        vec2 curr_grad;
        vec2 min_vec;
        f64 curr_t = -1;
        for (int i = 0; i < sol.size(); i++)
        {
            const auto& s = sol[i];
            if (s < 0 || s > 1) continue;
            vec2 point = {poly1.evaluate(s), poly2.evaluate(s)};
            f64 new_d = 
                (origin.first - point.first)*(origin.first - point.first) + 
                (origin.second - point.second)*(origin.second - point.second);
            if (new_d <= min_dist)
            {
                //curr_grad = curve_grad(points, s);
                curr_t = s + (delta) * math::utils::f64_eq(s, 0) 
                    - (delta) * math::utils::f64_eq(s, 1.);
                min_vec ={point.first - origin.first, point.second - origin.second};
            }
            min_dist = std::min(new_d, min_dist);
        }
        for (int i = 0; i < 4; i+=3)
        {
            f64 t = (1. - delta) * (i==3) + (delta) * (i==0);
            vec2 point = {poly1.evaluate(t), poly2.evaluate(t)};
            f64 new_d = 
                (origin.first - point.first)*(origin.first - point.first) + 
                (origin.second - point.second)*(origin.second - point.second);
            if (new_d <= min_dist)
            {
                curr_t = t;
                //curr_grad = curve_grad(points, (i==3));
                min_vec = {point.first - origin.first, point.second - origin.second};
            }
            min_dist = std::min(new_d, min_dist);
        }
        curr_t = std::clamp(curr_t, 1e-7, 1 - 1e-7);
        vec2 gb = {dpoly1.evaluate(curr_t),
            dpoly2.evaluate(curr_t)};
        i64 sgn = 
            (math::utils::f64_cmp(
                                  (-gb.second * min_vec.first+
                                       gb.first * min_vec.second),
                                  0) >= 0) * 2 - 1;
        return {std::sqrt(min_dist), sgn};
    }
    std::pair<f64, i64> cff_rasterizer::curve_intersect(vec2 ray_origin, u64 idx)
    {
        const auto& points = m_curves[idx];
        if (ray_origin.second < 
                std::min(
                    points[0].second, points[3].second)
                || ray_origin.second > 
                std::max(
                    points[0].second, points[3].second)
                || ray_origin.first > 
                std::max(
                    points[0].first,  points[3].first)
           )
        {
            return {std::numeric_limits<f64>().max(), false};
        }
        f64 a = -points[0].second + 3. * points[1].second - 3. * points[2].second + points[3].second;
        f64 b = 3. * (points[2].second - 2. * points[1].second + points[0].second);
        f64 c = 3. * (points[1].second - points[0].second);
        f64 d = points[0].second - ray_origin.second;
        auto cub = solve_cubic(a, b, c, d);
        f64 ret = std::numeric_limits<f64>().max();
        bool down = false;
        f64 eps = DEFAULT_RAST_EPS;
        i64 winding = 0;
        for (auto c : cub)
        {
            if (c < 0. || c > 1.) continue;
            vec2 pt = curve_interpoloate(points, c);
            if (pt.first >= ray_origin.first 
                    // && pt.first < ret
               )
            {
                if (pt.first < ret)
                {
                    ret = pt.first;
                }
                auto grad = curve_grad(idx, c);
                winding += (grad.second> 0) - (grad.second < 0);
                down = (grad.second > 0.);
            }
        }
        return {ret, winding};
    }
    cff_rasterizer::vec2 cff_rasterizer::curve_interpoloate(const std::array<vec2, 4>& points, f64 t)
    {
        f64 pt = 1. - t;
        f64 x = 
            pt * pt * pt * points[0].first + 
            3. * pt * pt * t * points[1].first + 
            3. * t * t * pt * points[2].first + 
            t * t * t * points[3].first;
        f64 y = 
            pt * pt * pt * points[0].second + 
            3. * pt * pt * t * points[1].second + 
            3. * t * t * pt * points[2].second + 
            t * t * t * points[3].second;
        return {x, y};
    }
    cff_rasterizer::vec2 cff_rasterizer::curve_grad(u64 idx, f64 t)
    {
        auto [p1, p2] = m_curve_polynomials_cache[idx];
        p1 = p1.derivative();
        p2 = p2.derivative();
        return {p1.evaluate(t), p2.evaluate(t)};
    }
    cff_rasterizer::vec2 cff_rasterizer::curve_norm(u64 idx, f64 t)
    {
        auto [p1, p2] = m_curve_polynomials_cache[idx];
        p1 = p1.derivative().derivative();
        p2 = p2.derivative().derivative();
        return {p1.evaluate(t), p2.evaluate(t)};
    }
    std::vector<f64> cff_rasterizer::solve_cubic(f64 a, f64 b, f64 c, f64 d)
    {
        auto arr = to_depressed(a, b, c, d);
        auto cub = solve_depressed_cubic(arr[0], arr[1]);
        for (auto& c : cub)
        {
            c -= (b/a)/3;
        }
        return cub;
    }
    std::array<f64, 2> cff_rasterizer::to_depressed(f64 a, f64 b, f64 c, f64 d)
    {
        f64 diff = (b/a)/3;
        std::array<f64, 2> coeffs = 
        {
            3 * diff * diff - (2 * diff ) * (b/a) + (c/a),
            -diff * diff * diff + diff * diff * (b/a) - diff * (c/a) + d/a
        };
        return coeffs;
    }
    std::vector<f64> cff_rasterizer::solve_depressed_cubic(f64 a, f64 b)
    {
        double disc = (b * b)/4.0 + (a * a * a)/27.0;
        std::vector<double> ret;
        double temp2 = -b/2.0;
        if (disc > DEFAULT_RAST_EPS)
        {
            double temp = sqrt(disc);
            ret= {
                pow(temp2 + temp,1.0/3.0) + pow(temp2-temp, 1.0/3.0),
            };
        }
        else if (std::abs(disc) <= DEFAULT_RAST_EPS)
        {
            double u = std::cbrt(temp2);
            ret= {
                2 * u
            };
            if (std::abs(u) > DEFAULT_RAST_EPS)
            {
                ret.push_back(-u);
            }
        }
        else
        {
            double r = 2.0 * sqrt(pow(temp2 * temp2 - disc, 1.0/3.0));
            double theta = atan2(sqrt(-disc), temp2);
            ret = {
                r * cos(theta/3),
                r * cos((theta + 2. * M_PI)/3),
                r * cos((theta + 4 * M_PI)/3),
            };
        }
        return ret;
    }
}}}}
