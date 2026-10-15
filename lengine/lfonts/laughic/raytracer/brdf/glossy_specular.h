#ifndef GLOSSY_SPECULAR_H
#define GLOSSY_SPECULAR_H
#include "brdf.h"
typedef struct GlossySpecularData
{
    f32 ks;
    u32 cs;
    f32 exp;
    Vector3d* latest_normal;
} GlossySpecularData;

u32 glossy_specular_f(void* data, Vector3d* in, Vector3d* out);
u32 glossy_specular_sample_f(void* data, Vector3d* in, Vector3d* out);
u32 glossy_specular_rho(void* data, Vector3d* out);

#define GLOSSY_SPECULAR_BRDF(s, n) ((BRDF)\
        {\
        .data = s,\
        .name = n,\
        .f = glossy_specular_f,\
        .sample_f = glossy_specular_sample_f,\
        .rho = glossy_specular_rho\
        })
#endif
