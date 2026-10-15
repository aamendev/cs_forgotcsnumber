#ifndef AMBIENT_LIGHT_H
#define AMBIENT_LIGHT_H
#include "light.h"
typedef struct AmbientLight
{
    u32 colour;
    f32 ls;
} AmbientLight;

Ray* amb_get_rays(Vector3d* in_position, void* data, u32* count);
u32 amb_get_radiance(Vector3d* in_position, void* data, float old_dist);
#define AMBIENT_LIGHT(s, n) ((Light)\
        {\
        .data = s,\
        .name = n,\
        .get_rays = amb_get_rays,\
        .get_radiance = amb_get_radiance\
        })
#endif
