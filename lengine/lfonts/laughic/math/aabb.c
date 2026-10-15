#include "aabb.h"
#include "vector3d.h"
#include <float.h>
#include <math.h>
#include <stdio.h>

void fswap (float* f1, float* f2)
{
    float temp;
    temp = *f1;
    *f1 = *f2;
    *f2 = temp;
}

static f32 min(f32 a, f32 b)
{
    return a * (a < b) + b * !(a < b);
}

static f32 max(f32 a, f32 b)
{
    return a * (a > b) + b * !(a > b);
}

float aabb_intersects(Ray* ray, void* data, Vector3d* normal)
{
    float t_near, t_far, t0, t1;

    AABB* box = (AABB*)data;  

    t0 = (box->min_coord.x - ray->base.x) * (1 / ray->direction.x);
    t1 = (box->max_coord.x - ray->base.x) * (1 / ray->direction.x);

    t_near = min(t0, t1);
    t_far = max(t0, t1);

    t0 = (box->min_coord.y - ray->base.y) * (1 / ray->direction.y);
    t1 = (box->max_coord.y - ray->base.y) * (1 / ray->direction.y);

    t_near = max(t_near, min(t0, t1));
    t_far = min(t_far, max(t0, t1));

    t0 = (box->min_coord.z - ray->base.z) * (1 / ray->direction.z);
    t1 = (box->max_coord.z - ray->base.z) * (1 / ray->direction.z);

    t_near = max(t_near, min(t0, t1));
    t_far = min(t_far, max(t0, t1));
    int cond = (t_far > max(0.0f, t_near));
    //int cond = (t_far >= 0.0f);
    if (cond && normal != NULL)
    {
        Vector3d c = mid_point(&box->min_coord, &box->max_coord);
        Vector3d hit_point = scale(&ray->direction, t_near);
        hit_point = add(&hit_point, &ray->base);
        Vector3d unit_map = to_unit_cube(&box->max_coord, &box->min_coord);
        hit_point = sub(&hit_point, &c);
        normal->x = (int)(hit_point.x / unit_map.x * 1.0010f);
        normal->y = (int)(hit_point.y / unit_map.y * 1.0010f);
        normal->z = (int)(hit_point.z / unit_map.z * 1.0010f);
        *normal = normalise(normal);
    }
    if (t_near < 0) t_near = t_far * (!box->bounding) + FLT_MIN * (box->bounding);
    return t_near * (cond) - !cond;
}
void aabb_get_bounding_extents(Traceable* t, Vector3d* min, Vector3d* max)
{
    AABB* box = (AABB*)t->data;
    *min = box->min_coord;
    *max = box->max_coord;
}
void aabb_get_bvh(Traceable* t, struct BVH* bvh)
{
    bvh->root = t;
    bvh->left = NULL;
    bvh->right = NULL;
}
float aabb_sdf(Traceable* t, Vector3d* p)
{
    AABB* box = (AABB*)t->data;

    Vector3d mid_box = mid_point(&box->min_coord, &box->max_coord);
    Vector3d rs = 
    {
        box->max_coord.x - mid_box.x,
        box->max_coord.y - mid_box.y,
        box->max_coord.z - mid_box.z,
    };
    Vector3d temp_p =
    {
        fabs(p->x - mid_box.x),
        fabs(p->y - mid_box.y),
        fabs(p->z - mid_box.z),
    };
    Vector3d diff = 
    {
        fmax((temp_p.x - rs.x), 0),
        fmax((temp_p.y - rs.y), 0),
        fmax((temp_p.z - rs.z), 0),
    };
    float max_comp = 
        diff.x * (diff.x >= diff.y && diff.x >= diff.z)
        + diff.y * (diff.y > diff.x && diff.y >= diff.z) 
        + diff.z * (diff.z > diff.x && diff.z > diff.y);
        
    //printf("%f\n", dist);
    
    float ret = magnitude(&diff) + fmin(max_comp, 0);
  //  printf("%f\n", ret);
    return ret;
}
