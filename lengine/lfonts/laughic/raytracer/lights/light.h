#ifndef LIGHT_H
#define LIGHT_H
#include "../../math/vector3d.h"
#include "../../graphics_util.h"
#include "../../logic_util.h"
#include "../../types.h"
#include "../../math/ray.h"
typedef struct Light
{
   void* data;
   char* name;
   Ray* (*get_rays)(Vector3d* in_position, void* data, u32* count);
   u32 (*get_radiance)(Vector3d* in_position, void* data, float old_dist);
}Light;

typedef struct MaterialData
{
    Light* ambient_light;
    Light* lights;
    u32 light_count;
} MaterialData;
#endif
