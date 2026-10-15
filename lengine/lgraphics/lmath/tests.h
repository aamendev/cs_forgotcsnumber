#ifndef MATH_TESTS_H
#define MATH_TESTS_H
#include "all.h"
#include <iostream>
#include "utils.h"
#define BEGIN_TEST(x) do{std::cerr << "running test " << x << "...\n";}while(0)
#define END_TEST(x) do{std::cerr << "test " << x << " success\n";}while(0)
namespace lina { namespace math { namespace tests{
    void v2d_tests();
    void v3d_tests();
    void mat2_tests();
    void mat3_tests();
    void mat4_tests();
    void transofmration_tests();
    void poly();
    void all();
}}}
#endif
