#ifndef SHADER_TESTS_H
#define SHADER_TESTS_H
#include <iostream>
#include "shader.h"
#include <cassert>
#define BEGIN_TEST(x) do{std::cerr << "running test " << x << "...\n";}while(0)
#define END_TEST(x) do{std::cerr << "test " << x << " success\n";}while(0)
#define OUT_OBJ (1)
namespace lina { namespace shader { namespace tests {
    void parsing();
    void parsing_and_compiling();
    void shader_emission();
    void shader_injection();
    void all();
}}}
#endif
