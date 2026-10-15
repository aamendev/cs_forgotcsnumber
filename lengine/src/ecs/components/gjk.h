#ifndef ECS_COLLIDER_GJK_H
#define ECS_COLLIDER_GJK_H
#include "collider.h"
#include <iostream>
#include <cmath>
#include <cfloat>
namespace lina { namespace ecs { namespace components{ namespace gjk {
#define COLLISION_EPSILON (0.0001f)

    typedef struct Simplex
    {
        u32 size = 0;
        std::vector<math::vector3d<f32>> points;

        inline void push(math::vector3d<f32>& v) {points.push_back(v); size++;}
        inline void push_front(math::vector3d<f32>& v) 
        {points.insert(points.begin(), v); size++;}
        inline void pop(math::vector3d<f32>& v) {points.pop_back(); size--;}
        inline math::vector3d<f32>& at(u32 idx) {return points[idx];}
        inline auto begin() {return points.begin();}
        inline auto end() {return points.end();}

        void set(std::vector<math::vector3d<f32>>&& v) 
        {
            size = v.size();
            points.clear();
            std::move(v.begin(), v.end(), std::back_inserter(points));
        }
        void clear()
        {size = 0; points.clear();}
    } Simplex;

    typedef struct Info
    {
        f32 depth = 0.0f;
        math::vector3d<f32> normal = {0, 0, 0};
        bool collided = false;
    } Info;

    math::vector3d<f32> support(collider* a, collider* b, const math::vector3d<f32>& d);
    bool gjk(collider* a, collider* b);
    Info gjk_and_info(collider* a, collider* b);
    bool check_next_simplex(Simplex&, math::vector3d<f32>& d);
    bool check_line(Simplex&, math::vector3d<f32>& d);
    bool check_tri(Simplex&, math::vector3d<f32>& d);
    bool check_tetra(Simplex&, math::vector3d<f32>& d);

    Info epa(Simplex& s, collider* a, collider* b);

    std::pair<std::vector<math::vector4d<f32>>, u32> normal_and_min(
            std::vector<math::vector3d<f32>>& verts,
            std::vector<u32>& indices);
    void add_unique_remove_dup(std::vector<std::pair<u32, u32>>& edges, std::vector<u32>& faces,
            u32 a, u32 b);
}}}}

#endif
