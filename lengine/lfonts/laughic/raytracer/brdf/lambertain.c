#include "lambertain.h"

u32 lambertain_f(void* data, Vector3d* in, Vector3d* out)
{
    LambertainData* d = (LambertainData*)data;    
    u8 comp[4];
    unpack(comp, &d->cd);
    u8 new_comp[4];
    for (int i = 0; i < 3; i++)
    {
        new_comp[i] = fmin(0xff, (u16)comp[i] * (d->kd * INV_PI));
    }
    new_comp[3] = comp[3];
    u32 ret = 0;
    pack(new_comp, &ret);
    return ret;
}
u32 lambertain_sample_f(void* data, Vector3d* in, Vector3d* out)
{
    LambertainData* d = (LambertainData*)data;    
    (void)d;
    return 0;
}
u32 lambertain_rho(void* data, Vector3d* out)
{
    LambertainData* d = (LambertainData*)data;    
    u8 comp[4];
    unpack(comp, &d->cd);
    u8 new_comp[4];
    for (int i = 0; i < 3; i++)
    {
        new_comp[i] = fmin(0xff, d->kd * (u16)comp[i]);
    }
    new_comp[3] = comp[3];
    u32 ret = 0;
    pack(new_comp, &ret);
    return ret;
}
