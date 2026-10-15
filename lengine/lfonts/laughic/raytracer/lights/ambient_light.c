#include "ambient_light.h"

Ray* amb_get_rays(Vector3d* in_position, void* data, u32* count)
{
    (void)in_position;
    (void)data;
    *count = 0;
    Ray* ray = malloc(sizeof(Ray));
    ray->direction  = (Vector3d){0, 0, 0};
    ray->base  = (Vector3d){0, 0, 0};
    return ray;
}

u32 amb_get_radiance(Vector3d* in_position, void* data, float old_dist)
{
    AmbientLight* pl = (AmbientLight*)data;
    u8 comps[4];
    unpack(comps, &pl->colour);
    for (int i = 0; i < 3; i++)
    {
        comps[i] *= pl->ls;
    }
    u32 ret_colour = 0;
    pack(comps, &ret_colour); 
    return ret_colour;
}
