#include "fish_eye.h"
#include <math.h>

Vector3d fish_eye_ray_direction(CameraData* data, Point2D* p)
{

    Vector3d w = sub(&data->eye, &data->look_at);
    w = normalise(&w);
    Vector3d u = cross(&data->up, &w);
    u = normalise(&u);
    Vector3d v = cross(&w, &u);

    float width = ((FishEyeData*)(data->extra))->width;
    float height = ((FishEyeData*)(data->extra))->height;
    float pixel_size = ((FishEyeData*)(data->extra))->pixel_size;
    float max_psi = ((FishEyeData*)(data->extra))->max_psi;
    Point2D pn = 
    {
        .x = 2.0 / (pixel_size * width) * p->x,
        .y = 2.0 / (pixel_size * height) * p->y
    };

    float r_squared = pn.x * pn.x + pn.y * pn.y;

    if (r_squared <= 1)
    {
        float r = sqrt(r_squared);
        float psi = r * max_psi * (PI / 180); 
        Vector3d scaled_u = scale(&u, sin(psi) * (pn.x / r));
        Vector3d scaled_v = scale(&v, sin(psi) * (pn.y / r));
        Vector3d scaled_w = scale(&w, - cos(psi));
        Vector3d add_1 = add(&scaled_u, &scaled_v);
        return add(&add_1, &scaled_w);
    }
    return (Vector3d){.x = 0, .y = 0 , .z = 0};
}
