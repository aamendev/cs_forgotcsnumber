#ifndef MESH_TESTS_H
#define MESH_TESTS_H
#include "obj_parser.h"
#include <iostream>
#include "halfedge.h"
#define BEGIN_TEST(x) do{std::cerr << "running test " << x << "...\n";}while(0)
#define END_TEST(x) do{std::cerr << "test " << x << " success\n";}while(0)
#define OUT_OBJ (1)
namespace lina { namespace mesh { namespace tests {
    void line_parse(); 
    void face_parse();

    void flip();
    void split();
    void collapse();
    void loop_subdivision();
    void quadratic_simplification();
    void remesh();
    void global_validation(halfedge::halfedge** e, i32* faces = nullptr, bool allow_twin = false);

    void all();
}}}
#endif
