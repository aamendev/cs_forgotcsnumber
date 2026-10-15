#include "glossy_specular.h"

u32 glossy_specular_f(void* data, Vector3d* in, Vector3d* out)
{
    u32 ret = 0;
    GlossySpecularData* d = (GlossySpecularData*)data;    
    u8 comp[4];
    Vector3d r = scale(out, -1);
    Vector3d inc = scale(d->latest_normal ,2 * dot(out, d->latest_normal));
    r = add(&r, &inc);
    f32 rdotw = dot(&r, in);
    if (rdotw > 0.0f)
    {
        unpack(comp, &d->cs);
        u8 new_comp[4];
        for (int i = 0; i < 3; i++)
        {
            new_comp[i] = fmin(0xff, d->ks * pow(rdotw, d->exp) * (u16)comp[i]);
        }
        new_comp[3] = comp[3];
        pack(new_comp, &ret);
    }
    return ret;
}
u32 glossy_specular_sample_f(void* data, Vector3d* in, Vector3d* out)
{
    GlossySpecularData* d = (GlossySpecularData*)data;    
    (void)d;
    return 0;
}
u32 glossy_specular_rho(void* data, Vector3d* out)
{
    GlossySpecularData* d = (GlossySpecularData*)data;    
    u8 comp[4];
    unpack(comp, &d->cs);
    u8 new_comp[4];
    for (int i = 0; i < 3; i++)
    {
        new_comp[i] = d->ks * (f32)comp[i];
    }
    new_comp[3] = comp[3];
    u32 ret = 0;
    pack(new_comp, &ret);
    return ret;
}
