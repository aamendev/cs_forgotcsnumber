#ifndef TRACEABLE_H
#define TRACEABLE_H
#include "ray.h"
#include "../types.h"
#include "vector3d.h"
#include "../colours.h"
#include "../raytracer/brdf/brdf.h"
#include "../experiments/noise_texture.h"

typedef struct Traceable
{
    struct BVH
    {
        struct Traceable* root;
        struct BVH* left;
        struct BVH* right;
    } m_bvh;

    void* data;
    char* name;
    u32 col;
    int id;
    BRDF ambient;
    BRDF diffuse;
    BRDF specular;
    int is_mirror;
    float (*intersects)(Ray* r, void* data, Vector3d* normal);
    void(*get_bounding_extents)(struct Traceable* t, Vector3d* min, Vector3d* max);
    void(*get_bvh)(struct Traceable* t, struct BVH* bvh);
    float (*sdf)(struct Traceable* t, Vector3d* p);
} Traceable;

typedef struct CombinedTraceable
{
    Traceable* t1;
    Traceable* t2;
} CombinedTraceable;
Traceable combine_traceable(Traceable* t1, Traceable* t2, f32 (*combined_sdf)(Traceable* t, Vector3d* p));
f32 union_sdf(Traceable* t1, Vector3d* p);
f32 inter_sdf(Traceable* t1, Vector3d* p);
f32 diff_sdf(Traceable* t1, Vector3d* p);
f32 xor_sdf(Traceable* t, Vector3d* p);
#endif
