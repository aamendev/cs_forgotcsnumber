#include "color_space.h"
#include "lmath/vector3d.h"
#include "lmath/utils.h"
#include "tests.h"
namespace lina { namespace graphics { namespace tests{
    void oklch_to_srgb()
    {
        BEGIN_TEST("oklch to srgb");
        math::vector3d<f32> col = {0.75, 0.1, 191.2};
        auto rgb = 
            colour_space::oklch_to_linear_rgb(std::move(col));
        rgb = colour_space::rgb_to_srgb(std::move(rgb));
        assert(math::utils::f64_eq(rgb.x, 85./255., 1e-2));
        assert(math::utils::f64_eq(rgb.y, 194./255., 1e-2));
        assert(math::utils::f64_eq(rgb.z, 189./255., 1e-2));

        col = {0.75, 0.3, 191.2};
        rgb = 
            colour_space::oklch_to_linear_rgb(std::move(col));
        rgb = colour_space::rgb_to_srgb(std::move(rgb));
        assert(math::utils::f64_eq(rgb.x, -1581./255., 1e-2));
        assert(math::utils::f64_eq(rgb.y, 224./255., 1e-2));
        assert(math::utils::f64_eq(rgb.z, 220./255., 1e-2));
        END_TEST("oklch to srgb");
    }
    void oklch_to_xyz()
    {
        BEGIN_TEST("oklch to xyz");
        math::vector3d<f32> col = {0.75, 0.1, 191.2};
        auto xyz = 
            colour_space::oklab_to_xyz(
                    colour_space::polar_to_linear(std::move(col)));
assert(math::utils::f64_eq(xyz.x, 0.3226, 1e-2));
        assert(math::utils::f64_eq(xyz.y, 0.4425, 1e-2));
        assert(math::utils::f64_eq(xyz.z, 0.5511, 1e-2));

        col = {0.75, 0.3, 191.2};
        xyz = 
            colour_space::oklab_to_xyz(
                    colour_space::polar_to_linear(std::move(col)));
        assert(math::utils::f64_eq(xyz.x, 0.1977, 1e-2));
        assert(math::utils::f64_eq(xyz.y, 0.4836, 1e-2));
        assert(math::utils::f64_eq(xyz.z, 0.7580, 1e-2));
        END_TEST("oklch to xyz");

    }
    void xyz_to_srgb()
    {
        BEGIN_TEST("xyz to srgb");
        math::vector3d<f32> white(0.95047/95.047, 1./100., 1.08883/108.883);
        math::vector3d<f32> col = {41.246, 21.267, 1.933};
        auto rgb = colour_space::xyz_to_linear_rgb(
                std::move(col * white));
        rgb = colour_space::rgb_to_srgb(std::move(rgb));
        assert(math::utils::f64_eq(rgb.x, 1., 1e-2));
        assert(math::utils::f64_eq(rgb.y, 0./255., 1e-2));
        assert(math::utils::f64_eq(rgb.z, 0./255., 1e-2));

        col = {0.25, 0.23, 0.68};
        rgb = colour_space::xyz_to_linear_rgb(
                std::move(col));
        rgb = colour_space::rgb_to_srgb(std::move(rgb));
        assert(math::utils::f64_eq(rgb.x, 96./255., 1e-2));
        assert(math::utils::f64_eq(rgb.y, 128./255., 1e-2));
        assert(math::utils::f64_eq(rgb.z, 216./255., 1e-2));

        col = {87., 50.32, 37.2};
        rgb = colour_space::xyz_to_linear_rgb(std::move(col * white));
        rgb = colour_space::rgb_to_srgb(std::move(rgb));
        assert(math::utils::f64_eq(rgb.x, 334./255., 1e-2));
        assert(math::utils::f64_eq(rgb.y, 96./255., 1e-2));
        assert(math::utils::f64_eq(rgb.z, 157./255., 1e-2));
        END_TEST("xyz to srgb");
    }
    void all()
    {
        oklch_to_xyz();
        xyz_to_srgb();
        oklch_to_srgb();
    }
}}}
