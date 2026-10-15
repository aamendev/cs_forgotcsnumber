#ifndef FONT_TESTS_H
#define FONT_TESTS_H
#include <iostream>
#include "font.h"
#include <cassert>
#define BEGIN_TEST(x) do{std::cerr << "running test " << x << "...\n";}while(0)
#define END_TEST(x) do{std::cerr << "test " << x << " success\n";}while(0)
namespace lina { namespace font { namespace tests {
    void load_file();
    void all();
}}}
#endif
