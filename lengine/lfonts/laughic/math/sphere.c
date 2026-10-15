#include "sphere.h"
#include "aabb.h"
#include "traceable.h"
#include "vector3d.h"
#include <math.h>
#include <stdlib.h>

/*int intersects(Ray *r, Sphere *s)
{
    Vector3d base_center = sub(&(s->center), &(r->base));
    if (dot(&base_center, &(r->direction)) < 0 &&
            squared_magnitude(&base_center) > s->r * s->r)
        return 0;
    Vector3d rejected = reject(&base_center, &(r->direction));
    return magnitude(&rejected) <= s->r;
}
*/

float sphere_intersects(Ray *r, void *data, Vector3d* normal)
{
    float rad = ((Sphere*)data)->r;
    Vector3d center = ((Sphere*)data)->center;

    Vector3d base_center = sub(&(center), &(r->base));
    float tca = dot(&base_center, &(r->direction));

    if (tca < 0 &&
            squared_magnitude(&base_center) > rad * rad)
        return -1;
    Vector3d rejected = reject(&base_center, &(r->direction));
    float thc = sqrt(rad * rad - squared_magnitude(&rejected));
    f32 val = tca - thc;
    if (val < 0) val = tca + thc;
    f32 ret = (magnitude(&rejected) <= rad) * val - !(magnitude(&rejected) <= rad);
    if (ret > -1 && normal != NULL)
    {
        Vector3d hit_point = scale(&r->direction, val);
        hit_point = add(&hit_point, &r->base);
        *normal = sub(&hit_point, &center);
        *normal = normalise(normal);
    }
    return ret; 
}


float sphere_sdf(Traceable* t, Vector3d* p)
{
    Sphere* s = (Sphere*)t->data;
    Vector3d diff = sub(&(s->center), p);
    Vector3d norm = normalise(&diff);
    Vector3d scaled = scale(&norm, s->r);
    scaled = add(&scaled, &s->center);
    float temp = magnitude(&diff);
   // f32 noise_val =  s->r * 0.2f * perPointHybridMultiFractal(scaled, 0.8f, 2.2f, 12, 1.7f);
   f32 noise_val = 0.0f;
    return temp - (s->r) + noise_val;
}
void sphere_get_bounding_extents(Traceable* t, Vector3d* min, Vector3d* max)
{
    Sphere* s = (Sphere*)t->data;
    *min = (Vector3d){s->center.x - s->r, s->center.y - s->r, s->center.z - s->r};
    *max = (Vector3d){s->center.x + s->r, s->center.y + s->r, s->center.z + s->r};
}
void sphere_get_bvh(Traceable* t, struct BVH* bvh)
{
    /*AABB* bounding_box = malloc(sizeof(AABB));
    sphere_get_bounding_extents(t, &bounding_box->min_coord, &bounding_box->max_coord);
    Traceable* bounding = malloc(sizeof(Traceable));
    *bounding = AABB_TRACE(bounding_box, WHITE, "bounding");
    */
    bvh->root = t;
    bvh->left = NULL;
    bvh->right = NULL;
}
