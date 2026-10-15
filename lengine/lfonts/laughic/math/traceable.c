#include "traceable.h"
#include "vector3d.h"
f32 union_sdf(Traceable* t, Vector3d* p)
{
    CombinedTraceable* d = (CombinedTraceable*)t->data;

    f32 sdf1 = d->t1->sdf(d->t1, p);
    f32 sdf2 = d->t2->sdf(d->t2, p);
    return fmin(sdf1, sdf2);
}
f32 inter_sdf(Traceable* t, Vector3d* p)
{
    CombinedTraceable* d = (CombinedTraceable*)t->data;

    f32 sdf1 = d->t1->sdf(d->t1, p);
    f32 sdf2 = d->t2->sdf(d->t2, p);
    return fmax(sdf1, sdf2);
}
f32 diff_sdf(Traceable* t, Vector3d* p)
{
    CombinedTraceable* d = (CombinedTraceable*)t->data;

    f32 sdf1 = d->t1->sdf(d->t1, p);
    f32 sdf2 = d->t2->sdf(d->t2, p);
    return fmax(-sdf1, sdf2);
}
f32 xor_sdf(Traceable* t, Vector3d* p)
{
    CombinedTraceable* d = (CombinedTraceable*)t->data;

    f32 sdf1 = d->t1->sdf(d->t1, p);
    f32 sdf2 = d->t2->sdf(d->t2, p);
    return fmax(fmin(sdf1, sdf2), -fmax(sdf1, sdf2));
}

Traceable combine_traceable(Traceable* t1, Traceable* t2, 
        f32 (*combined_sdf)(Traceable* t, Vector3d* t2))
{
    Traceable t;
    CombinedTraceable* c1  = malloc(sizeof(CombinedTraceable));
    c1->t1 = t1;
    c1->t2 = t2;
    t.data = c1;
    t.sdf = combined_sdf;
    return t;
}
