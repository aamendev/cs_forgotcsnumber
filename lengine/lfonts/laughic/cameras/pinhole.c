#include "./pinhole.h"
#include "camera.h"
#include <stdio.h>

Vector3d pinhole_ray_direction(CameraData* data, Point2D* p)
{
    float d = ((PinholeData*)data->extra)->d;
    Vector3d w = sub(&data->eye, &data->look_at);
    w = normalise(&w);
    Vector3d u = cross(&data->up, &w);
    u = normalise(&u);
    Vector3d v = cross(&w, &u);
    
    Vector3d scaled_u = scale(&u, p->x);
    Vector3d scaled_v = scale(&v, p->y);
    Vector3d scaled_w = scale(&w, -d);
    Vector3d add_1 = add(&scaled_u, &scaled_v);
    Vector3d add_2 = add(&add_1, &scaled_w);
    return normalise(&add_2);
}
