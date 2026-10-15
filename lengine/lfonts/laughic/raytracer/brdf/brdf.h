#ifndef BRDF_H
#define BRDF_H
#include "../../types.h"
#include "../../math/vector3d.h"
#include "../../samplers/sampler.h"
#include "../../graphics_util.h"
typedef struct BRDF 
{
    void* data;
    char* name;
    Sampler* sampler;
    u32 (*f)(void* data, Vector3d* in, Vector3d* out);
    u32 (*sample_f)(void* data, Vector3d* in, Vector3d* out);
    u32 (*rho)(void* data, Vector3d* out);
} BRDF;
#endif
