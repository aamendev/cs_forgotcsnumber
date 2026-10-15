#ifndef COLOUR_SPACE_FUNCTIONS_H
#define COLOUR_SPACE_FUNCTIONS_H
#include "lmath/mat3.h"
#include "lmath/vector3d.h"
namespace lina { namespace graphics { namespace colour_space {
    template <typename T>
        inline math::vector3d<T> clamp_channels(
                math::vector3d<T>&& col, 
                math::vector3d<T>&& max_vals = {(T)1., (T)1., (T)1.}, 
                math::vector3d<T>&& min_vals = {(T)0., (T)0., (T)0.})
        {
            col.x = std::clamp(col.x, min_vals.x, max_vals.x);
            col.y = std::clamp(col.y, min_vals.y, max_vals.y);
            col.z = std::clamp(col.z, min_vals.z, max_vals.z);
            return col;
        }
    template <typename T>
        inline void clamp_channels(math::vector3d<T>& col, 
                math::vector3d<T>&& max_vals = {(T)1., (T)1., (T)1.}, 
                math::vector3d<T>&& min_vals = {(T)0., (T)0., (T)0.})
        {
            col.x = std::clamp(col.x, min_vals.x, max_vals.x);
            col.y = std::clamp(col.y, min_vals.y, max_vals.y);
            col.z = std::clamp(col.z, min_vals.z, max_vals.z);
        }
    template <typename T>
        inline math::mat3<T> xyz_to_linear_rgb_matrix()
        {
            math::mat3<T> m(
                    math::vector3d<T>((T)3.2404542, (T)-1.5371385, (T)-0.4985314),
                    math::vector3d<T>((T)-0.9692660,  (T)1.8760108,  (T)0.0415560),
                    math::vector3d<T>((T)0.0556434, (T)-0.2040259,  (T)1.0572252));
            return m;
        }
    template <typename T>
        inline std::pair<math::mat3<T>, math::mat3<T>> xyz_to_oklab_matrices()
        {
            math::mat3<T> m1 = math::mat3<T>(
                    math::vector3d<T>((T)+0.8189330101,(T)+0.0329845436,(T)+0.0482003018),
                    math::vector3d<T>((T)+0.3618667424,(T)+0.9293118715,(T)+0.2643662691),
                    math::vector3d<T>((T)-0.1288597137,(T)+0.0361456387,(T)+0.6338517070)).get_transpose();
            math::mat3<T> m2 = math::mat3<T>(
                    math::vector3d<T>((T)+0.2104542553,(T)+1.9779984951,(T)+0.0259040371),
                    math::vector3d<T>((T)+0.7936177850,(T)-2.4285922050,(T)+0.7827717662),
                    math::vector3d<T>((T)-0.0040720468,(T)+0.4505937099,(T)-0.8086757660)
                    ).get_transpose();
            return {m1, m2};
        }
    template <typename T>
        inline math::vector3d<T> polar_to_linear(math::vector3d<T>&& polar)
        {
            const T pi = 3.14159265358979323846;
            T h = polar.z * pi / (T)180.0;
            T a = polar.y * std::cos(h);
            T b = polar.y * std::sin(h);
            return {polar.x, a, b};
        }

    template <typename T>
        T linear_to_screen(T c)
        {
            if (c <= (T)0.0031308)
                return c * (T)12.92;
            else
                return (T)1.055 * std::pow(c, 1./2.4) - 0.055;
        }
    template <typename T>
        inline math::vector3d<T> oklab_to_xyz(math::vector3d<T>&& oklab)
        {
            std::pair<math::mat3<T>, math::mat3<T>> mats = xyz_to_oklab_matrices<T>();
            auto m1inv = mats.first.get_inverse();
            auto m2inv = mats.second.get_inverse();
            auto v1 = m2inv * oklab;
            v1.x *= (v1.x * v1.x);
            v1.y *= (v1.y * v1.y);
            v1.z *= (v1.z * v1.z);
            return m1inv * v1;
        }

    template <typename T>
        inline math::vector3d<T> lab_to_xyz(math::vector3d<T>&& lab)
        {
            f64 kappa = 24389./27.;
            f64 eps = 216./24389.;
            math::vector3d<T> white(0.95047, 1., 1.08883);
            math::vector3d<T> xs(0., 0., 0.);
            math::vector3d<T> fs((lab.x + 16.) / 116., (lab.x + 16.)/116., 0.);
            fs.x = fs.y + lab.y / 500.;
            fs.z = fs.y - lab.z / 200.;

            if (fs.x * fs.x * fs.x > eps)
            {
                xs.x = fs.x * fs.x * fs.x;
            }
            else 
            {
                xs.x = (116. * fs.x - 16.)/kappa;
            }

            if (lab.x > kappa * eps)
            {
                xs.y = fs.y * fs.y * fs.y;
            }
            else 
            {
                xs.y = lab.x / kappa;
            }

            if (fs.z * fs.z * fs.z > eps)
            {
                xs.z = fs.z * fs.z * fs.z;
            }
            else 
            {
                xs.z = (116. * fs.z - 16.)/kappa;
            }
            return xs * white;
        }
    template <typename T>
        inline math::vector3d<T> xyz_to_linear_rgb(math::vector3d<T>&& xyz)
        {
            return xyz_to_linear_rgb_matrix<T>() * xyz;
        }
    template <typename T> 
        inline math::vector3d<T> rgb_to_srgb(math::vector3d<T>&& srgb)
        {
            return math::vector3d<T>(
                    linear_to_screen(srgb.x),
                    linear_to_screen(srgb.y),
                    linear_to_screen(srgb.z)
                    );
        }
    template <typename T>
        inline math::vector3d<T> lch_to_rgb(math::vector3d<T>&& lch)
        {
            auto lab = polar_to_linear(std::move(lch));
            math::vector3d<T> rgb = lab_to_xyz(std::move(lab));
            rgb = xyz_to_linear_rgb(std::move(rgb));
            return rgb_to_srgb(std::move(rgb));
        }
    template <typename T>
        inline math::vector3d<T> oklch_to_linear_rgb(math::vector3d<T>&& oklch)
        {
            auto oklab = polar_to_linear(std::move(oklch));
            oklab = oklab_to_xyz(std::move(oklab));
            return xyz_to_linear_rgb(std::move(oklab));
        }
    template <typename T>
        inline math::vector3d<T> oklch_to_rgb(math::vector3d<T>&& oklch)
        {
            auto rgb = oklch_to_linear_rgb(std::move(oklch));
            rgb = rgb_to_srgb(std::move(rgb));
            return clamp_channels(std::move(rgb));
        }
    template <typename T>
        inline T max_chroma(T lightness, T hue)
        {
            T max_chroma = (T)200.;
            T min_chroma = (T)0.;
            T eps = 1e-2;
            while (max_chroma - min_chroma > eps)
            {
                auto curr_chroma = (max_chroma + min_chroma) / 2.;
                auto srgb = oklch_to_linear_rgb(math::vector3d<T>(lightness, curr_chroma, hue));
                if (
                        srgb.x > 1. || srgb.x < 0. ||
                        srgb.y > 1. || srgb.y < 0. ||
                        srgb.z > 1. || srgb.z < 0.)
                {
                    max_chroma = curr_chroma;
                }
                else 
                {
                    min_chroma = curr_chroma;
                }
            }
            return min_chroma;
        }
}}}
#endif
