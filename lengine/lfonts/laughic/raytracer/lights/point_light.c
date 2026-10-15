#include "point_light.h"
#include "light.h"

Ray* pl_get_rays(Vector3d* in_position, void* data, u32* count)
{
    PointLight* pl = (PointLight*)data;
    Ray* ray = malloc(sizeof(Ray));
    ray->base = pl->pos;
    ray->direction = sub(in_position, &pl->pos);
    ray->direction = normalise(&ray->direction);
    *count = 1;
    return ray;
}

u32 pl_get_radiance(Vector3d* in_position, void* data, float old_dist)
{
    PointLight* pl = (PointLight*)data;
    Vector3d diff = sub(&pl->pos, in_position);
    f32 total_mag = (magnitude(&diff) + old_dist);
    f32 atten = 1.0f/ (total_mag * total_mag);
    u8 comps[4];
    unpack(comps, &pl->colour);
    for (int i = 0; i < 3; i++)
    {
        comps[i] = fmin(0xff, (u16)comps[i] * (pl->ls * atten));
    }
    u32 ret_colour = 0;
    pack(comps, &ret_colour); 
    return ret_colour;
}
