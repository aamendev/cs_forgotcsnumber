#ifndef LAMBTERTIAN_H
#define LAMBTERTIAN_H
#include "brdf.h"
typedef struct LambertainData
{
    f32 kd;
    u32 cd;
} LambertainData;

u32 lambertain_f(void* data, Vector3d* in, Vector3d* out);
u32 lambertain_sample_f(void* data, Vector3d* in, Vector3d* out);
u32 lambertain_rho(void* data, Vector3d* out);

#define LAMBERTAIN_BRDF(s, n) ((BRDF)\
        {\
        .data = s,\
        .name = n,\
        .f = lambertain_f,\
        .sample_f = lambertain_sample_f,\
        .rho = lambertain_rho\
        })
#endif
