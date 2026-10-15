#ifndef CPP_UTILS_TESTS_H
#define CPP_UTILS_TESTS_H
#include "all.h"
#include <iostream>
#include <string>
#define BEGIN_TEST(x) do{std::cerr << "running test " << x << "...\n";}while(0)
#define END_TEST(x) do{std::cerr << "test " << x << " success\n";}while(0)
namespace lina { namespace cpp_utils { namespace tests{
    void string();
    void all();
}}}
#endif
