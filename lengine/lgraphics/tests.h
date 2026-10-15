#ifndef GRAPHICS_TESTS_H
#define GRAPHICS_TESTS_H
#include <iostream>
#include <cassert>
#define BEGIN_TEST(x) do{std::cerr << "running test " << x << "...\n";}while(0)
#define END_TEST(x) do{std::cerr << "test " << x << " success\n";}while(0)
namespace lina { namespace graphics { namespace tests {
    void oklch_to_srgb();
    void xyz_to_srgb();
    void oklch_to_xyz();
    void all();
}}}
#endif
