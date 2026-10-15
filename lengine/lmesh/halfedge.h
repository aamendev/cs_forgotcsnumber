#ifndef HALF_EDGE_H
#define HALF_EDGE_H
#include "lmath/all.h"
#include "lmath/lossy_conversions.h"
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <vector>
namespace lina { namespace mesh { namespace halfedge{
    struct vertex;
    struct halfedge;
    struct face;
    struct vertex 
    {
        math::vector3d<f64> pos;
        halfedge* hedge = nullptr;
    };
    struct halfedge 
    {
        vertex* tip = nullptr;
        halfedge* next = nullptr;
        halfedge* twin = nullptr;
        face* f = nullptr;
    };
    struct face 
    {
        halfedge* hedge = nullptr;
    };
    void subdivide(halfedge* e, int iter = 1);
    void simplify(halfedge** e, int iter = 1);
    void remesh(halfedge** e, f64 lambda, int iter=1);


    bool loop_subdivision(halfedge**);
    bool quadratic_simplification(halfedge**, int iter = 1);
    void iso_remesh(halfedge **e, f64 lambda);

    bool flip_edge(halfedge**);
    bool split_edge(
            halfedge**,  
            std::unordered_set<halfedge*>& newly_added, 
            std::unordered_set<vertex*>& newly_added_vertex, 
            std::array<f64, 4> arr = {0.5, 0.5, 0., 0.});
    bool split_edge(halfedge**,  std::array<f64, 4> arr = {0.5, 0.5, 0., 0.});
    bool collapse_edge(halfedge**, std::array<f64, 2> arr = {0.5, 0.5});
    // helpers
    static int get_degree(vertex* v);
    static math::mat4<f64> get_initial_plane_errors(vertex* v);
    static int highest_neighbour_degree(vertex* v);
    static std::set<vertex*> get_neighbours(vertex* v);
    static std::vector<vertex*> get_neighbours_vector(vertex* v);
    static std::vector<halfedge*> get_half_edges(vertex* v);
    static void update_helf_edges_vertex(vertex* vold, vertex* vnew);
    static bool shared_neighbour_condition(vertex* v1, vertex* v2);
    static f64 get_area(vertex* v);
    static math::vector3d<f64> get_normal(vertex* v);

}}}
#endif
