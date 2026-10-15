#ifndef POINT_LIGHT_H
#define POINT_LIGHT_H
#include "light.h"
#include <stdlib.h>
typedef struct PointLight
{
    u32 shadow_colour;
    Vector3d pos;
    u32 colour;
    f32 ls;
    Vector3d* target;
} PointLight;

Ray* pl_get_rays(Vector3d* in_position, void* data, u32* count);
u32 pl_get_radiance(Vector3d* in_position, void* data, float old_dist);
#define POINT_LIGHT(s, n) ((Light)\
        {\
        .data = s,\
        .name = n,\
        .get_rays = pl_get_rays,\
        .get_radiance = pl_get_radiance\
        })
#endif
